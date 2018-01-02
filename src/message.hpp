#ifndef YODLE_MESSAGE_HPP_
#define YODLE_MESSAGE_HPP_

#include <cassert>
#include <cstddef>
#include <sstream>
#include <vector>
#include <memory>

namespace yodle {

struct Message {
    Message();
    Message(const int kind);

    // User defined bbmessage kind
    int kind;

    // Body size (without headers)
    std::size_t size;
    std::stringstream ss;

    std::vector<char> getData();
};

template<typename KIND>
std::shared_ptr<Message> createMessage(const KIND kind)
{
    return std::make_shared<Message>(static_cast<decltype(Message::kind)>(kind));
}
} // namespace yodle

#endif // YODLE_MESSAGE_HPP_
