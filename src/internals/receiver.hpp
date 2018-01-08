#ifndef YODLE_INTERNALS_RECEIVER_HPP_
#define YODLE_INTERNALS_RECEIVER_HPP_

#include "../types.hpp"

#include <vector>
#include <functional>
#include <memory>
#include <ev++.h>

namespace yodle {

struct Message;

namespace internals {

/**
 * Receives data
 */
struct Receiver {
    typedef std::function<void(const std::shared_ptr<Message> message)> MessageHandler;

    Receiver(int fd,
             struct ev_loop* evLoop,
             const MessageHandler& onMessage);
    ~Receiver();

    Receiver(const Receiver&) = delete;
    Receiver(Receiver&&) = delete;
    Receiver& operator=(const Receiver&) = delete;

    /**
     * Starts receiving
     */
    void start();

    /**
     * Stops receiving
     */
    void stop();

    /**
     * @return is this connection closed
     */
    bool isClosed();

    /**
     * Shutdowns the connection
     */
    void shutdown();

private:

    // New data on socket
    void onInput(ev::io& w, int revents);

    void parse(const std::vector<char>& data);

    // Waits for new data
    ev::io mInputWatcher;

    // Socket's fd
    int mFD;

    // The request that's being received
    std::shared_ptr<Message> mMessage;

    // Called when a valid request is ready
    MessageHandler mOnMessage;

    // Parser specific
    enum struct State {
        START,
        KIND,
        SIZE,
        BODY,
        END,
    };

    State mState;

    // Meaning differs
    std::size_t mBytesRead;
};

} // namespace internals
} // namespace everest

#endif // YODLE_INTERNALS_RECEIVER_HPP_mBytesRead