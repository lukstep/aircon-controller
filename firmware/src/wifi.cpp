#include "wifi.hpp"
#include "logger.hpp"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "rtc.hpp"

namespace wifi
{
bool init(const char *ssid, const char *password)
{
    *logger << rtc::getTime() << " [INF] Start WiFi connection\n";

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND))
    {
        *logger << rtc::getTime() << " [ERR] Failed to init!\n";
        return false;
    }

    cyw43_arch_enable_sta_mode();

    *logger << rtc::getTime() << " [INF] try to connect ssid:" << ssid << "\n";

    constexpr int32_t connectionTimeoutMs = 10'000;
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK,
                                           connectionTimeoutMs))
    {
        *logger << rtc::getTime() << " [ERR] Failed to connect to network!\n";
        return false;
    }

    *logger << rtc::getTime() << " [INF] Connected to " << ssid << "\n";
    return true;
}

void deinit() { cyw43_arch_deinit(); }

} // namespace wifi