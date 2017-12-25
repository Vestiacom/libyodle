#include "message.hpp"
#include <cstring>

namespace yodle {

// Gets
std::vector<char> Message::getData()
{
    // Get body size
    ss.seekg(0, std::ios::end);
    std::size_t size = ss.tellg();
    size += sizeof(Message::kind) + sizeof(Message::size);
    ss.seekg(0, std::ios::beg);

    // Fill continous table
    auto data = std::vector<char>(size);
    std::memcpy(&data[0], &kind, sizeof(Message::kind));
    std::memcpy(&data[sizeof(Message::kind)], &size, sizeof(Message::size));
   
    auto body = ss.str();
    std::memcpy(&data[sizeof(Message::kind + sizeof(Message::size))], body.c_str(), body.size());

    return data;
}

} // namespace yodle

