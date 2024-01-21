#include "rs485.hpp"
#include "hardware/gpio.h"
#include "logger.hpp"
#include "pico/stdlib.h"
#include "pico/types.h"
#include <chrono>
#include <cstdint>

namespace RS485
{

void on_uart_rx()
{
    static int32_t bytesReceived = 0;
    static int32_t frame = 0;
    static bool packet_tx = false;
    static constexpr uint8_t packetPrefix = 0xAA;
    static constexpr uint8_t packetPostfix = 0x55;

    while (uart_is_readable(uart1))
    {
        uint8_t byte = uart_getc(uart1);
        if (byte == packetPrefix and !packet_tx)
        {
            *logger << "RS485 #" << frame << " [" << Logger::Radix_t::Hex;
            packet_tx = true;
        }
        *logger << byte << " ";
        bytesReceived++;
        if (byte == packetPostfix and packet_tx)
        {
            *logger << "] " << Logger::Radix_t::Decimal << bytesReceived << " bytes rx\n";
            frame++;
            bytesReceived = 0;
            packet_tx = false;
        }
    }
}

void init()
{
    constexpr size_t baudrate = 4800;
    constexpr size_t nBits = 8;
    constexpr size_t nStopBits = 1;
    *logger << "baudrate:" << baudrate << " nBits:" << nBits << " nStopBits:" << nStopBits << "\n";
    uart_init(uart1, baudrate);
    gpio_set_function(RS485::UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RS485::UART1_RX_PIN, GPIO_FUNC_UART);
    int __unused actual = uart_set_baudrate(uart1, baudrate);
    uart_set_hw_flow(uart1, false, false);
    uart_set_format(uart1, nBits, nStopBits, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart1, true);
    irq_set_exclusive_handler(UART1_IRQ, RS485::on_uart_rx);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(uart1, true, false);
}
} // namespace RS485