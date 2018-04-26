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
	  mOnMessage(onMessage),
	  mState(State::START)
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
	LOGD("Starting");
	mInputWatcher.start(mFD, ev::READ);
}

void Receiver::stop()
{
	LOGD("Stopping");
	mInputWatcher.stop();
}

void Receiver::shutdown()
{
	LOGD("Shutting down");
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
	try {

		LOGD("Received input");
		if (EV_ERROR & revents) {
			LOGW("Unspecified error in input callback: " <<  std::strerror(errno));
			shutdown();
			return;
		}

		// Make this configurable
		// TODO: Avoid allocating every time
		std::vector<char> buf(512);

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

		if (received == 0) {
			LOGD("Connection closed by peer");
			shutdown();
			return;
		}

		// This won't reallocate any data, just a trick to work with a vector
		buf.resize(received);

		parse(buf);
	}
	catch (const std::exception& e) {
		LOGE("Unexpected exception: " << e.what());
	}
	catch (...) {
		LOGE("Unexpected exception");
	}
}

void Receiver::parse(const std::vector<char>& data)
{
	LOGD("Parsing message, size: " << data.size());
	for (size_t i = 0; i < data.size();) {
		// LOGD("PARSER: State" << (int)mState);
		switch (mState) {
		case State::START: {
			LOGD("PARSER: Message start");
			mMessage = std::make_shared<Message>();

			// Prepare for kind
			mBytesRead = 0;
			mState = State::KIND;
			break;
		}
		case State::KIND: {
			reinterpret_cast<char*>(&mMessage->kind)[mBytesRead] = data[i];

			++mBytesRead;
			if (mBytesRead >= sizeof(Message::kind)) {
				LOGD("PARSER: Kind: " << mMessage->kind);

				// Prepare for size
				mBytesRead = 0;
				mState = State::SIZE;
			}

			++i;
			break;
		}
		case State::SIZE: {
			// LOGD("PARSER: Size byte: " <<  std::to_string((int)data[i]));
			// LOGD("PARSER: Size read: " << std::to_string(mBytesRead));
			reinterpret_cast<char*>(&mMessage->size)[mBytesRead] = data[i];

			++mBytesRead;
			++i;
			if (mBytesRead >= sizeof(Message::size)) {
				// Prepare for body
				mBytesRead = 0;
				LOGD("PARSER: Size: " << mMessage->size);
				if (mMessage->size != 0) {
					mState = State::BODY;
				}
				else {
					mState = State::END;
					goto state_end;
				}
			}

			break;
		}
		case State::BODY: {
			// LOGD("PARSER: Body expected: " << mMessage->size << " read: " << mBytesRead);

			std::size_t bodyLeft = mMessage->size - mBytesRead;
			// bodyLeft = bodyLeft > data.size() - i;
			std::size_t dataLeft = data.size() - i;


			std::size_t toRead = bodyLeft > dataLeft ? dataLeft : bodyLeft;
			std::copy_n(data.begin() + i, toRead, std::ostream_iterator<char>(mMessage->ss));
			mBytesRead += toRead;

			if (mMessage->size != mBytesRead) {
				// Body not yet ready
				LOGD("BODY not ready");
				return;
			}

			mState = State::END;
			i += toRead;

			// NO BREAK HERE
			// Move to the next state right away
			// break;
		}
state_end:
		case State::END: {
			LOGD("PARSER: Message end");
			mOnMessage(mMessage);
			mState = State::START;
			break;
		}
		}
	}

	LOGD("Parsed all data");

}

} // namespace internals
} // namespace everest

