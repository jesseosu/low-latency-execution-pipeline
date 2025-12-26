#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "cacheline.hpp"

namespace ll {

// Requires Capacity be power of two.
constexpr bool is_pow2(std::size_t x) noexcept { return x && ((x & (x - 1)) == 0); }

template <typename T, std::size_t Capacity>
class SpscQueue {
  static_assert(Capacity >= 2, "Capacity must be >= 2");
  static_assert(is_pow2(Capacity), "Capacity must be a power of two");
  static_assert(std::is_trivially_copyable_v<T>,
                "SpscQueue expects trivially copyable T for max speed");

public:
  SpscQueue() noexcept : head_(0), tail_(0) {}

  // Producer thread
  bool try_push(const T& item) noexcept {
    const std::size_t tail = tail_.load(std::memory_order_relaxed);
    const std::size_t next = (tail + 1) & (Capacity - 1);

    if (next == head_.load(std::memory_order_acquire)) {
      return false; // full
    }

    buffer_[tail] = item;
    tail_.store(next, std::memory_order_release);
    return true;
  }

  // Consumer thread
  bool try_pop(T& out) noexcept {
    const std::size_t head = head_.load(std::memory_order_relaxed);

    if (head == tail_.load(std::memory_order_acquire)) {
      return false; // empty
    }

    out = buffer_[head];
    head_.store((head + 1) & (Capacity - 1), std::memory_order_release);
    return true;
  }

  bool empty() const noexcept {
    return head_.load(std::memory_order_acquire) ==
           tail_.load(std::memory_order_acquire);
  }

  std::size_t approx_size() const noexcept {
    const std::size_t h = head_.load(std::memory_order_acquire);
    const std::size_t t = tail_.load(std::memory_order_acquire);
    return (t - h) & (Capacity - 1);
  }

private:
  alignas(CACHELINE_SIZE) std::atomic<std::size_t> head_;
  alignas(CACHELINE_SIZE) std::atomic<std::size_t> tail_;
  alignas(CACHELINE_SIZE) T buffer_[Capacity];
};

} // namespace ll
