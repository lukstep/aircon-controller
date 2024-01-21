pico_sdk_init()

add_executable(aircon main.cpp wifi.cpp logger.cpp ntp.cpp rtc.cpp rs485.cpp)

pico_enable_stdio_usb(aircon 1)
pico_enable_stdio_uart(aircon 1)
pico_add_extra_outputs(aircon)

target_include_directories(aircon PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(aircon PRIVATE   
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    )

target_link_libraries(aircon 
                      pico_stdlib
                      pico_util
                      hardware_rtc
                      pico_cyw43_arch_lwip_threadsafe_background)
