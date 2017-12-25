#include "receiver.hpp"
#include "logger.hpp"

#include <ev++.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <vector>
#include <algorithm>
#include <iterator>


namespace yodle {
namespace internals {

Receiver::Receiver(int fd,
                   struct ev_loop* evLoop,
                   const MessageHandler& onMessage)
    : mInputWatcher(evLoop),
      mFD(fd),
      mOnMessage(onMessage)
{
    if (!evLoop) {
        THROW("ev_loop is null");
    }

    if (mFD < 0) {
        THROW("bad fd");
    }

    mInputWatcher.set<Receiver, &Receiver::onInput>(this);

}

Receiver::~Receiver()
{
    shutdown();
}

void Receiver::start()
{
    mInputWatcher.start(mFD, ev::READ);
}

void Receiver::stop()
{
    mInputWatcher.stop();
}

void Receiver::shutdown()
{
    if (mFD < 0) {
        return;
    }

    // No communication with the socket is possible after shutdown
    stop();
    if (-1 == ::shutdown(mFD, SHUT_RD)) {
        LOGW("shutdown() failed with: " <<  std::strerror(errno));
    }
    mFD = -1;

    mMessage.reset();
}

bool Receiver::isClosed()
{
    return mFD == -1;
}

void Receiver::onInput(ev::io& w, int revents)
{
    if (EV_ERROR & revents) {
        LOGW("Unspecified error in input callback: " <<  std::strerror(errno));
        shutdown();
        return;
    }

    // Make this configurable
    std::vector<char> buf(1024);

    ssize_t received = ::read(w.fd, buf.data(), buf.size());
    if (received < 0) {
        if (errno == ECONNRESET) {
            // Connection reset by peer.
            shutdown();
            return;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            LOGD("Neglected error when reading the Receiver's socket:" <<  std::strerror(errno));
            return;
        }

        LOGE("Error when reading the Receiver's socket:" <<  std::strerror(errno));
        shutdown();
        return;
    }

    // This won't reallocate any data, just a trick to work with a vector
    buf.resize(received);

    parse(buf);

}

void Receiver::parse(const std::vector<char>& data)
{
    for (size_t i = 0; i < data.size();) {
        switch (mState) {
        case State::START: {
            mMessage = std::make_shared<Message>();

            // Prepare for kind
            mBytesRead = 0;
            mState = State::KIND;
            break;
        }
        case State::KIND: {
            int shift = (data[i] << (8 * mBytesRead));
            int mask = 0xff << shift;
            mMessage->kind = (~mask &  mMessage->kind) | shift;

            ++mBytesRead;
            if (mBytesRead > sizeof(Message::kind)) {
                // Prepare for size
                mBytesRead = 0;
                mState = State::SIZE;
            }

            ++i;
            break;
        }
        case State::SIZE: {
            int shift = (data[i] << (8 * mBytesRead));
            int mask = 0xff << shift;
            mMessage->size = (~mask &  mMessage->size) | shift;

            ++mBytesRead;
            if (mBytesRead > sizeof(Message::size)) {
                // Prepare for body
                mBytesRead = 0;
                if (mMessage->size == 0) {
                    mState = State::BODY;
                }
                else {
                    mState = State::END;
                }
            }

            ++i;
            break;
        }
        case State::BODY: {
            std::size_t bodyLeft = mMessage->size - mBytesRead;
            // bodyLeft = bodyLeft > data.size() - i;
            std::size_t dataLeft = data.size() - i;


            std::size_t toRead = bodyLeft > dataLeft ? dataLeft : bodyLeft;            
            std::copy_n(data.begin() + i, toRead, std::ostream_iterator<char>(mMessage->ss));
            mBytesRead += toRead;

            mState = State::END;

            // NO BREAK HERE
            // Move to the next state right away
            // break;
        }
        case State::END: {
            mOnMessage(mMessage);
            mState = State::START;
            break;
        }
        }
    }
}

} // namespace internals
} // namespace everest

