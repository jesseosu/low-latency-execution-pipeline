#pragma once
#include <cstdint>

namespace ll {

using InstrumentId = std::uint32_t;
using OrderId      = std::uint64_t;
using SeqNo        = std::uint64_t;

// Fixed-point: e.g. price in "ticks" or "1e-4" units (you decide in later phases)
using Price = std::int32_t;
using Qty   = std::uint32_t;

enum class Side : std::uint8_t { Buy = 0, Sell = 1 };
enum class MsgType : std::uint8_t { Add = 0, Cancel = 1, Trade = 2 };

} // namespace ll
