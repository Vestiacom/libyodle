#ifndef YODLE_INTERNALS_LOGGER_HPP_
#define YODLE_INTERNALS_LOGGER_HPP_

#include "../types.hpp"
#include <iostream>
#include <sstream>

namespace yodle {

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
            yodle::internals::gLogCallback(yodle::LogLevel::YODLE_LOG_LEVEL_ERROR, msg__.str());              \
        }                                                                                     \
        throw std::runtime_error(msg__.str());                                                \
    } while (0)

#define LOGE(MESSAGE) LOG(yodle::LogLevel::YODLE_LOG_LEVEL_ERROR, MESSAGE)
#define LOGW(MESSAGE) LOG(yodle::LogLevel::YODLE_LOG_LEVEL_WARN, MESSAGE)
#define LOGI(MESSAGE) LOG(yodle::LogLevel::YODLE_LOG_LEVEL_INFO, MESSAGE)

#if !defined(NDEBUG)

#define LOGD(MESSAGE) LOG(yodle::LogLevel::YODLE_LOG_LEVEL_DEBUG, MESSAGE)
#define LOGT(MESSAGE) LOG(yodle::LogLevel::YODLE_LOG_LEVEL_TRACE, MESSAGE)

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
