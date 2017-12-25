#ifndef YODLE_INTERNALS_LOGGER_HPP_
#define YODLE_INTERNALS_LOGGER_HPP_

#include "../types.hpp"
#include <iostream>
#include <sstream>

namespace yodle {

std::string toString(const LogLevel logLevel);

namespace internals {
extern thread_local LogCallback gLogCallback;

} // namespace internals

/// Generic logging macro. Needs mLogger callback to be present.
#define LOG(LEVEL, MESSAGE)                                                 \
    do {                                                                    \
        std::ostringstream msg__;                                           \
        msg__ << MESSAGE << " [" << __FILENAME__ << ":" << __LINE__ << "]"; \
        if (yodle::internals::gLogCallback) {                               \
            yodle::internals::gLogCallback(LEVEL, msg__.str());             \
        }                                                                   \
    } while (0)

#define THROW(MESSAGE)                                                                        \
    do {                                                                                      \
        std::ostringstream msg__;                                                             \
        msg__ << "libyodle: " << MESSAGE << " [" << __FILENAME__ << ":" << __LINE__ << "]";   \
        if (yodle::internals::gLogCallback) {                                                 \
            yodle::internals::gLogCallback(yodle::LogLevel::ERROR, msg__.str());              \
        }                                                                                     \
        throw std::runtime_error(msg__.str());                                                \
    } while (0)

#define LOGE(MESSAGE) LOG(yodle::LogLevel::ERROR, MESSAGE)
#define LOGW(MESSAGE) LOG(yodle::LogLevel::WARN, MESSAGE)
#define LOGI(MESSAGE) LOG(yodle::LogLevel::INFO, MESSAGE)

#if !defined(NDEBUG)

#define LOGD(MESSAGE) LOG(yodle::LogLevel::DEBUG, MESSAGE)
#define LOGT(MESSAGE) LOG(yodle::LogLevel::TRACE, MESSAGE)

#else

#define LOGD(MESSAGE) \
    do {              \
    } while (0)
#define LOGT(MESSAGE) \
    do {              \
    } while (0)

#endif

} // namespace yodle

#endif // YODLE_INTERNALS_LOGGER_HPP_
