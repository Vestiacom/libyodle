#ifndef YODLE_SOCKET_PAIR_HPP_
#define YODLE_SOCKET_PAIR_HPP_

#include <cassert>
#include <cstddef>
#include <sstream>

namespace yodle {
namespace internals {

struct Message {

    int mKind;
    std::size_t mSize;
    std::stringstream mSS;

    int getData();
};

} // namespace internals
} // namespace yodle

#endif // YODLE_SOCKET_PAIR_HPP_
