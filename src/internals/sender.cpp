#include "sender.hpp"
#include "logger.hpp"

#include <map>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#include <sys/socket.h>

namespace yodle {
namespace internals {


Sender::Sender(int fd, struct ev_loop* evLoop)
    : mOutputWatcher(evLoop),
      mFD(fd),
      mIsClosing(false)
{
    if (!evLoop) {
        THROW("ev_loop is null");
    }

    if (mFD < 0) {
        THROW("bad fd");
    }

    mOutputWatcher.set<Sender, &Sender::onOutput>(this);
}

Sender::~Sender()
{
    shutdown();
}

void Sender::start()
{
    if (!mMessages.empty()) {
        mOutputWatcher.start(mFD, ev::WRITE);
    }
}

void Sender::stop()
{
    mOutputWatcher.stop();
}

void Sender::shutdown()
{
    if (mFD < 0) {
        return;
    }

    // No communication with the socket is possible after shutdown
    stop();

    if (-1 == ::shutdown(mFD, SHUT_WR)) {
        LOGW("shutdown() failed with: " <<  std::strerror(errno));
    }
    mFD = -1;

    while (!mMessages.empty()) {
        mMessages.pop();
    }

    mOutputBuffer.clear();
    mOutputBuffer.shrink_to_fit();
}

bool Sender::isClosed()
{
    return mFD == -1;
}

void Sender::fillBuffer()
{
    auto& message = *mMessages.front();
    mOutputBuffer = message.getData();
}

void Sender::onOutput(ev::io& w, int revents)
{
    // cout << "onOutput" << endl;

    if (EV_ERROR & revents) {
        LOGE("Unspecified error in output callback: " <<  std::strerror(errno));
        shutdown();
        return;
    }


    // Ensure output buffer has any data to send
    if (mOutputBufferPosition >= mOutputBuffer.size()) {
        // No data to send in mOutputBuffer.

        if (mIsClosing) {
            // And it was the last message in this connection
            shutdown();
            return;
        }


        if (mMessages.empty()) {
            // And there's no more responses to send.
            // Pause sending and free the buffer.
            mOutputBufferPosition = 0;
            mOutputBuffer.clear();
            mOutputWatcher.stop();
            return;
        }

        // Fill mOutputBuffer with the next serialized Response from the queue.
        mOutputBufferPosition = 0;
        mOutputBuffer.resize(0);
        fillBuffer();
    }

    ssize_t n  = ::write(w.fd,
                         &mOutputBuffer[mOutputBufferPosition],
                         mOutputBuffer.size() - mOutputBufferPosition);
    if (n >= 0) {
        mOutputBufferPosition += n;
    }
    else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        // Neglected errors
    }
    else {
        if (errno == ECONNRESET) {
            // Connection reset by peer.
            shutdown();
            return;
        }
        LOGE("write() failed with: " <<  std::strerror(errno));
        shutdown();
        return;
    }

    if (n == 0) {
        shutdown();
    }
}

void Sender::send(const std::shared_ptr<yodle::Message> response)
{
    mMessages.push(response);

    // Ensure sending data is switched on
    mOutputWatcher.start(mFD, ev::WRITE);
}

} // namespace internals
} // namespace yodle
