#include "logger.hpp"
#include "internals/logger.hpp"

#include <iostream>
#include <sstream>

namespace yodle {

std::string toString(const LogLevel logLevel)
{
	switch (logLevel) {
	case LogLevel::YODLE_LOG_LEVEL_ERROR:
		return "ERROR";
	case LogLevel::YODLE_LOG_LEVEL_WARN:
		return "WARN ";
	case LogLevel::YODLE_LOG_LEVEL_INFO:
		return "INFO ";
	case LogLevel::YODLE_LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LogLevel::YODLE_LOG_LEVEL_TRACE:
		return "TRACE";
	default:
		return "UNKNOWN";
	}
}

void setLogger(const LogCallback& logCallback)
{
	internals::gLogCallback = logCallback;
}

} // namespace yodle




