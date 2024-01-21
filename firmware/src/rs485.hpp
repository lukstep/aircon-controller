#pragma once

#include <cstddef>
#include <cstdint>

namespace RS485
{
static constexpr int32_t UART1_TX_PIN = 8;
static constexpr int32_t UART1_RX_PIN = 9;
void init();
} // namespace RS485
