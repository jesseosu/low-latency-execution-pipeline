#pragma once
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <vector>

namespace ll {

// Very simple fixed-capacity pool (single-thread use for Phase 1).
template <typename T>
class FixedPool {
  static_assert(std::is_trivially_destructible_v<T>,
                "FixedPool assumes trivially destructible T for simplicity");

public:
  explicit FixedPool(std::size_t capacity)
      : storage_(capacity), free_(capacity) {
    for (std::size_t i = 0; i < capacity; ++i) free_[i] = capacity - 1 - i;
  }

  T* acquire() noexcept {
    if (free_top_ == 0) return nullptr;
    const std::size_t idx = free_[--free_top_];
    return &storage_[idx];
  }

  void release(T* ptr) noexcept {
    if (!ptr) return;
    const std::size_t idx = static_cast<std::size_t>(ptr - storage_.data());
    free_[free_top_++] = idx;
  }

  std::size_t capacity() const noexcept { return storage_.size(); }
  std::size_t available() const noexcept { return free_top_; }

private:
  std::vector<T> storage_;
  std::vector<std::size_t> free_;
  std::size_t free_top_ = free_.size();
};

} // namespace ll
