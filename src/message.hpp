#ifndef YODLE_SOCKET_PAIR_HPP_
#define YODLE_SOCKET_PAIR_HPP_

#include <cassert>
#include <cstddef>
#include <sstream>
#include <vector>

namespace yodle {

struct Message {

    int kind;
    std::size_t size;
    std::stringstream ss;

    std::vector<char> getData();
};

} // namespace yodle

#endif // YODLE_SOCKET_PAIR_HPP_
