#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

#include "pico/util/datetime.h"

class Logger
{
  public:
    using Radix_t = enum RadixEnum { Binary = 2, Octal = 8, Decimal = 10, Hex = 16 };
    enum class loggerType
    {
        uart,
        udp,
    };

  protected:
    loggerType type;
    Radix_t mRadixSetting;

  public:
    Logger(loggerType type) : type(type), mRadixSetting(Radix_t::Hex){};

    Logger &operator<<(const char *string)
    {
        const size_t length = std::strlen(string);
        write(string, length);

        return *this;
    }

    template <typename T>
        requires std::integral<T>
    Logger &operator<<(T value)
    {
        static_assert(sizeof(T) < sizeof(int64_t), "Type bigger than 64 bits is not supported");
        constexpr size_t prefixAndNullSize = 3;
        constexpr size_t maxSize = std::numeric_limits<T>::digits + prefixAndNullSize;
        char string[maxSize];

        static const std::array<char, 16> digits{'0', '1', '2', '3', '4', '5', '6', '7',
                                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        const uint32_t radix = static_cast<uint32_t>(mRadixSetting);
        size_t fieldindex = 0;
        int32_t first = 0;

        switch (mRadixSetting)
        {
        case RadixEnum::Binary:
            string[0] = '0';
            string[1] = 'b';
            fieldindex += 2;
            first += 2;
            break;
        case RadixEnum::Octal:
            string[0] = '0';
            string[1] = 'o';
            fieldindex += 2;
            first += 2;
            break;
        case RadixEnum::Decimal:
            break;
        case RadixEnum::Hex:
            string[0] = '0';
            string[1] = 'x';
            fieldindex += 2;
            first += 2;
            break;
        }

        uint32_t remainder;
        do
        {
            remainder = value % radix;
            string[fieldindex] = digits[remainder];
            value /= radix;
            fieldindex += 1;
        } while (value != 0);

        // reverse order
        int32_t last = fieldindex - 1;
        while ((last - first) > 0)
        {
            char tmp = string[last];
            string[last] = string[first];
            string[first] = tmp;
            first += 1;
            last -= 1;
        }

        string[fieldindex + 1] = '\0';

        write(string, fieldindex);

        return *this;
    }

    Logger &operator<<(Radix_t radix)
    {
        mRadixSetting = radix;

        return *this;
    }

    Logger &operator<<(datetime_t time)
    {
        constexpr size_t maxSize = 19;
        char string[maxSize];
        const size_t length = sprintf(string, "%02d-%02d-%04d %02d:%02d:%02d", time.day, time.month,
                                      time.year, time.hour, time.min, time.sec);
        write(string, length);

        return *this;
    }

  private:
    void write(const char *string, size_t length);
};

extern class Logger *logger;