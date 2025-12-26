#pragma once
#include <cstdint>

#if defined(_WIN32)
  #define NOMINMAX
  #include <windows.h>
#else
  #include <ctime>
#endif

namespace ll {

// Monotonic-ish clock in nanoseconds.
// On Linux we'll use clock_gettime(CLOCK_MONOTONIC_RAW).
// On Windows we use QueryPerformanceCounter for a monotonic high-res clock.
inline std::uint64_t now_ns() noexcept {
#if defined(_WIN32)
  static LARGE_INTEGER freq = []{
    LARGE_INTEGER f{};
    QueryPerformanceFrequency(&f);
    return f;
  }();

  LARGE_INTEGER counter{};
  QueryPerformanceCounter(&counter);

  // Convert to ns carefully to avoid overflow
  const long double seconds = static_cast<long double>(counter.QuadPart) /
                              static_cast<long double>(freq.QuadPart);
  return static_cast<std::uint64_t>(seconds * 1'000'000'000.0L);
#else
  timespec ts{};
  ::clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return static_cast<std::uint64_t>(ts.tv_sec) * 1'000'000'000ULL +
         static_cast<std::uint64_t>(ts.tv_nsec);
#endif
}

} // namespace ll
