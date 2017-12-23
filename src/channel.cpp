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

void Channel::send(const int kind, const std::shared_ptr<std::vector<char>> data)
{
    mSender->send(kind, data);
}

void Channel::on(const int kind, const MessageHandler& cb)
{
    mHandlers.push_back(std::make_pair(kind, cb));
}

void Channel::onMessage(const int kind, const std::shared_ptr<std::vector<char>> data)
{
    // Find callback
    auto it = std::find_if(mHandlers.begin(), mHandlers.end(), [](const std::pair<int, MessageHandler>& element) {
        return element.first == kind;
    });

    if (it == mHandlers.end()) {
        LOGW("Unknown message type");
        return;
    }

    // Handle message
    try {
        it->second(data);
    }
    catch (const std::exception& e) {
        LOGE("Got exception in parsing HTTP: " << e.what());
        return;
    }
}

} // namespace yodle
