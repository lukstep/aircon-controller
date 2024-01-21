#include "rtc.hpp"
#include <chrono>

#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/types.h"

namespace rtc
{
void init()
{
    rtc_init();
    datetime_t init = {
        .year = 1970, .month = 1, .day = 1, .dotw = 0, .hour = 0, .min = 0, .sec = 0};
    rtc_set_datetime(&init);
    sleep_us(64);
}

datetime_t getTime()
{
    static datetime_t t;
    rtc_get_datetime(&t);
    return t;
}

void setTime(datetime_t *time)
{
    rtc_set_datetime(time);
    sleep_us(64);
}

} // namespace rtc

namespace std::chrono
{
time_point<high_resolution_clock> high_resolution_clock::now()
{
    return time_point(nanoseconds(1000ull * static_cast<nanoseconds::rep>(time_us_64())));
}
} // namespace std::chrono