#include "channel.hpp"

#include <vector>
#include <ev++.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <algorithm>
#include "internals/logger.hpp"

namespace yodle {

Channel::Channel(int fd,
                 struct ev_loop* evLoop)
    : mFD(fd)
{
    if (!evLoop) {
        THROW("ev_loop is null");
    }

    if (mFD < 0) {
        THROW("bad fd");
    }

    using namespace std::placeholders;

    mReceiver = std::unique_ptr<internals::Receiver>(new internals::Receiver(fd,
                                                                             evLoop,
                                                                             std::bind(&Channel::onMessage, this, _1)));
    mSender = std::unique_ptr<internals::Sender>(new internals::Sender(fd, evLoop));
}

Channel::~Channel()
{
    shutdown();
}

void Channel::start()
{
    mReceiver->start();
    mSender->start();
}

void Channel::stop()
{
    mReceiver->stop();
    mSender->stop();
}

int Channel::getFD()
{
    return mFD;
}

void Channel::shutdown()
{
    if (mFD < 0) {
        return;
    }

    mReceiver->shutdown();
    mSender->shutdown();

    if (-1 == ::close(mFD)) {
        LOGW("close() failed with: " << std::strerror(errno));
    }

    mFD = -1;
}

bool Channel::isClosed()
{
    return mReceiver->isClosed() || mSender->isClosed() || mFD == -1;
}

void Channel::send(const std::shared_ptr<yodle::Message> message)
{
    mSender->send(message);
}

void Channel::onMessage(const std::shared_ptr<yodle::Message> message)
{
    // Find callback
    auto it = std::find_if(mHandlers.begin(), mHandlers.end(), [&message](const std::pair<int, MessageHandler>& element) {
        return element.first == message->kind;
    });

    if (it == mHandlers.end()) {
        LOGW("Unknown message type");
        return;
    }

    // Handle message
    try {
        it->second(message);
    }
    catch (const std::exception& e) {
        LOGE("Exception in message handler: " << e.what());
        return;
    }
}

} // namespace yodle
