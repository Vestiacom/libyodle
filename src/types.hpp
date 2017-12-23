#ifndef YODLE_TYPES_HPP_
#define YODLE_TYPES_HPP_

#include <functional>
#include "message.hpp"

namespace yodle {

/// Log level used in the logging callback
enum class LogLevel : int {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
};
typedef std::function<void(LogLevel, const std::string& message)> LogCallback;

typedef std::function<void(const std::shared_ptr<Message> message)> MessageHandler;

typedef std::shared_ptr<std::vector<char>> Data;

} // namespace yodle

#endif // YODLE_TYPES_HPP_