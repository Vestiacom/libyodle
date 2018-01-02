#include "message.hpp"
#include <cstring>
#include <iostream>
namespace yodle {


Message::Message()
    : kind(0),
      size(0)
{}

Message::Message(const int kind)
    : kind(kind),
      size(0)
{}
// Gets
std::vector<char> Message::getData()
{
    // Get body size
    auto body = ss.str();
    size = body.size();

    // Fill continous table
    std::vector<char> data(size + sizeof(Message::kind) + sizeof(Message::size));
    std::memcpy(&data[0], &kind, sizeof(Message::kind));
    std::memcpy(&data[sizeof(Message::kind)], &size, sizeof(Message::size));

    if (size != 0) {
        std::memcpy(&data[sizeof(Message::kind) + sizeof(Message::size)], body.c_str(), size);
    }

    return data;
}

} // namespace yodle

