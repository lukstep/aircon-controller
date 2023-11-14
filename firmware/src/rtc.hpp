#pragma once
#include "pico/util/datetime.h"

namespace rtc
{

void init();
datetime_t getTime();
void setTime(datetime_t *time);

} // namespace rtc