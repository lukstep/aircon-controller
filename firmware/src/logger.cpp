#include "logger.hpp"

class Logger *logger = nullptr;

void Logger::write(const char *string, [[maybe_unused]] size_t length)
{
    // TODO add logging via UDP
    switch (type)
    {
    case loggerType::uart:
        printf("%s", string);
        break;
    case loggerType::udp:
        printf("UDP not available yet!");
        break;
    }
}
