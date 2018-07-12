#include "logger.hpp"
#include "internals/logger.hpp"

#include <iostream>
#include <sstream>

namespace yodle {

std::string toString(const LogLevel logLevel)
{
	switch (logLevel) {
	case LogLevel::ERROR:
		return "ERROR";
	case LogLevel::WARN:
		return "WARN ";
	case LogLevel::INFO:
		return "INFO ";
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::TRACE:
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




