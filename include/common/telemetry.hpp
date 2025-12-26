#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>

namespace ll {

struct LatencyStats {
  std::uint64_t count = 0;
  std::uint64_t min_ns = 0;
  std::uint64_t max_ns = 0;
  std::uint64_t p50_ns = 0;
  std::uint64_t p99_ns = 0;
};

class LatencyCollector {
public:
  explicit LatencyCollector(std::size_t reserve = 1'000'000) {
    samples_.reserve(reserve);
  }

  void record(std::uint64_t ns) {
    samples_.push_back(ns);
  }

  LatencyStats summarize() {
    LatencyStats s{};
    if (samples_.empty()) return s;

    std::sort(samples_.begin(), samples_.end());
    s.count = samples_.size();
    s.min_ns = samples_.front();
    s.max_ns = samples_.back();
    s.p50_ns = percentile(50);
    s.p99_ns = percentile(99);
    return s;
  }

  void reset() { samples_.clear(); }

private:
  std::uint64_t percentile(int p) const {
    const std::size_t n = samples_.size();
    const std::size_t idx = (static_cast<std::size_t>(p) * (n - 1)) / 100;
    return samples_[idx];
  }

  std::vector<std::uint64_t> samples_;
};

} // namespace ll
