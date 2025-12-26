#include <iostream>
#include <thread>
#include <cstdint>
#include "common/spsc_queue.hpp"
#include "common/time.hpp"
#include "common/telemetry.hpp"

struct Msg {
  std::uint64_t t0;
  std::uint64_t seq;
};

int main() {
  constexpr std::size_t CAP = 1 << 14;
  ll::SpscQueue<Msg, CAP> q;
  ll::LatencyCollector lat(500000);

  constexpr std::uint64_t N = 500000;

  std::thread prod([&]{
    for (std::uint64_t i = 0; i < N; ++i) {
      Msg m{ ll::now_ns(), i };
      while (!q.try_push(m)) {
        // spin
      }
    }
  });

  std::thread cons([&]{
    Msg m{};
    std::uint64_t got = 0;
    while (got < N) {
      if (q.try_pop(m)) {
        const std::uint64_t t1 = ll::now_ns();
        lat.record(t1 - m.t0);
        ++got;
      }
    }
  });

  prod.join();
  cons.join();

  const auto s = lat.summarize();
  std::cout << "SPSC end-to-end (producer stamp -> consumer now) ns: "
            << "count=" << s.count
            << " min=" << s.min_ns
            << " p50=" << s.p50_ns
            << " p99=" << s.p99_ns
            << " max=" << s.max_ns
            << "\n";
  return 0;
}
