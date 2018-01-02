#ifndef YODLE_SOCKET_PAIR_HPP_
#define YODLE_SOCKET_PAIR_HPP_

#include <cassert>
#include <cstddef>
#include <sstream>
#include <vector>

namespace yodle {

struct Message {
    // User defined bbmessage kind
    int kind;

    // Body size (without headers)
    std::size_t size;
    std::stringstream ss;

    std::vector<char> getData();
};

} // namespace yodle

#endif // YODLE_SOCKET_PAIR_HPP_
