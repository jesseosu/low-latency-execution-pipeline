#pragma once
#include <cstddef>
#include <cstdint>

namespace ll {

inline constexpr std::size_t CACHELINE_SIZE = 64;

template <typename T>
struct alignas(CACHELINE_SIZE) CacheAligned {
  T value;
};

constexpr std::size_t cacheline_pad(std::size_t n) noexcept {
  return (n + (CACHELINE_SIZE - 1)) & ~(CACHELINE_SIZE - 1);
}

} // namespace ll
