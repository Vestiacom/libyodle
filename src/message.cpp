#include "message.hpp"
#include <cstring>
namespace yodle {

// Gets
std::vector<char> Message::getData()
{
    // Get body size
    ss.seekg(0, std::ios::end);
    size = ss.tellg();
    ss.seekg(0, std::ios::beg);

    // Fill continous table
    std::vector<char> data(size + sizeof(Message::kind) + sizeof(Message::size));
    std::memcpy(&data[0], &kind, sizeof(Message::kind));
    std::memcpy(&data[sizeof(Message::kind)], &size, sizeof(Message::size));

    auto body = ss.str();
    if (body.size() != 0) {
        std::memcpy(&data[sizeof(Message::kind + sizeof(Message::size))], body.c_str(), body.size());
    }

    return data;
}

} // namespace yodle

