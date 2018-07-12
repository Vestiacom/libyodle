#include "logger.hpp"
#include "../logger.hpp"

namespace yodle {

namespace internals {

LogCallback thread_local gLogCallback = [](const yodle::LogLevel level, const std::string& msg)
{
    std::cout << "[" << yodle::toString(level) << "]\t" << msg << std::endl;
    std::cout.flush();
};

} // namespace internals

} // namespace yodle