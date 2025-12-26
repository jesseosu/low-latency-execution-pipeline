#include <iostream>
#include <cstdint>
#include "common/time.hpp"
#include "common/telemetry.hpp"

int main() {
  ll::LatencyCollector c(2'000'00);

  // Measure cost of calling now_ns() back-to-back
  for (int i = 0; i < 200000; ++i) {
    const std::uint64_t t0 = ll::now_ns();
    const std::uint64_t t1 = ll::now_ns();
    c.record(t1 - t0);
  }

  const auto s = c.summarize();
  std::cout << "now_ns() delta (ns): "
            << "count=" << s.count
            << " min=" << s.min_ns
            << " p50=" << s.p50_ns
            << " p99=" << s.p99_ns
            << " max=" << s.max_ns
            << "\n";
  return 0;
}
