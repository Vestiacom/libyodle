#ifndef YODLE_LOGGER_HPP_
#define YODLE_LOGGER_HPP_

#include "types.hpp"

namespace yodle {

std::string toString(const LogLevel logLevel);

void setLogger(const LogCallback& logCallback);

} // namespace yodle

#endif // YODLE_LOGGER_HPP_
