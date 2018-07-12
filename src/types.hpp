#ifndef YODLE_TYPES_HPP_
#define YODLE_TYPES_HPP_

#include "message.hpp"

#include <functional>
#include <memory>

namespace yodle {

/// Log level used in the logging callback
enum class LogLevel : int {
    YODLE_LOG_LEVEL_TRACE,
    YODLE_LOG_LEVEL_DEBUG,
    YODLE_LOG_LEVEL_INFO,
    YODLE_LOG_LEVEL_WARN,
    YODLE_LOG_LEVEL_ERROR,
};
typedef std::function<void(LogLevel, const std::string& message)> LogCallback;

typedef std::function<void(const std::shared_ptr<Message> message)> MessageHandler;

typedef std::shared_ptr<std::vector<char>> Data;

} // namespace yodle

#endif // YODLE_TYPES_HPP_