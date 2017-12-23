#ifndef YODLE_CHANNEL_HPP_
#define YODLE_CHANNEL_HPP_


#include "message.hpp"
#include "internals/sender.hpp"
#include "internals/receiver.hpp"

#include <vector>
#include <functional>
#include <memory>
#include <ev++.h>

namespace yodle {

/**
 * Wraps together all data used by one Channel.
 */
struct Channel: std::enable_shared_from_this<Channel> {

    Channel(int fd,
            struct ev_loop* evLoop);
    ~Channel();

    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;

    /**
     * Starts receiving and sending data
     */
    void start();

    /**
     * Stops receiving and sending data
     */
    void stop();

    /**
     * Write data to the output buffer.
     * Will not block.
     */
    void send(const int kind, const std::shared_ptr<std::vector<char>> data);

    /**
     * Register message handler
     */
    void on(const int kind, const MessageHandler& cb);

    /**
     * @return underlying fd
     */
    int getFD();

    /**
     * @return is this Channel closed
     */
    bool isClosed();

private:
    // Callbacks for different message kinds
    std::vector<std::pair<int, MessageHandler>> mHandlers;

    // Handles receiving messages
    std::unique_ptr<internals::Receiver> mReceiver;

    // Handles sending messages
    std::unique_ptr<internals::Sender> mSender;

    // Shutdowns the Channel
    void shutdown();

    // Handle message
    void onMessage(const int kind, const std::shared_ptr<std::vector<char>> data);

    // Socket's fd
    int mFD;
};

} // namespace yodle

#endif // YODLE_CHANNEL_HPP_