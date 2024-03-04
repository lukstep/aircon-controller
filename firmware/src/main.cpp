#include <cstdint>

#include "lwipopts.h"

#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/types.h"

#include "logger.hpp"
#include "ntp.hpp"
#include "rs485.hpp"
#include "rtc.hpp"
#include "wifi.hpp"

#include <chrono>
using namespace std::chrono;
int main()
{
    stdio_init_all();
    rtc::init();
    logger = new Logger(Logger::loggerType::uart);

    constexpr int32_t RED_LED_PIN = 2;
    constexpr int32_t YELLOW_LED_PIN = 3;
    constexpr int32_t GREEN_LED_PIN = 4;
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, true);
    gpio_init(YELLOW_LED_PIN);
    gpio_set_dir(YELLOW_LED_PIN, true);
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, true);

    bool is_wifi_connected = wifi::init(WIFI_SSID, WIFI_PASSWORD);

    if (!is_wifi_connected)
    {
        gpio_put(RED_LED_PIN, 1);
    }
    else
    {
        *logger << rtc::getTime() << " [INF] Initialization finished\n";

        ntp::Client ntpClient;
        if (ntpClient.syncTime())
        {
            rtc::setTime(ntpClient.getTime());
            *logger << rtc::getTime() << " RTC synced!\n";
        }
    }
    RS485::init();

    while (5)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        gpio_put(GREEN_LED_PIN, 0);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        gpio_put(GREEN_LED_PIN, 1);
        sleep_ms(250);
    }
}