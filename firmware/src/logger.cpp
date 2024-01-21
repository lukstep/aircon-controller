#include "logger.hpp"
#include "pico/stdlib.h"
#include "pico/types.h"
#include <cstdint>

class Logger *logger = nullptr;

void Logger::write(const char *string, [[maybe_unused]] size_t length)
{
    // TODO add logging via UDP
    switch (type)
    {
    case loggerType::uart:
        for (size_t i = 0; i < length; i++)
        {
            putchar(string[i]);
        }
        break;
    case loggerType::udp:
        printf("UDP not available yet!");
        break;
    }
}
