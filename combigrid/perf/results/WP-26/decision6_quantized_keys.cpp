// WP-26 decision 6 (scratch measurement, not part of the build). Build: g++ -std=c++23 -O3 -march=native
// decision6_quantized_keys.cpp -o q && taskset -c 4 ./q   (XXH3 of exact vs. mantissa-rounded keys, ns per key)
#define XXH_INLINE_ALL
#include <xxhash.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>
#include <bit>
static uint64_t sink;
template <bool QUANT>
double bench(size_t d) {
  const size_t n = 1 << 14;
  std::vector<double> pts(n * d);
  std::mt19937_64 rng(1);
  for (auto& x : pts) x = std::ldexp(double(rng() >> 44), -20) * 3.7 - 1.3;
  uint64_t key[32];
  auto t0 = std::chrono::steady_clock::now();
  uint64_t acc = 0;
  const int reps = 400;
  for (int r = 0; r < reps; r++)
    for (size_t i = 0; i < n; i++) {
      const double* x = &pts[i * d];
      asm volatile("" : : "r"(x) : "memory");
      if constexpr (QUANT) {
        for (size_t k = 0; k < d; k++) {
          uint64_t b = std::bit_cast<uint64_t>(x[k]);
          key[k] = (b + (uint64_t{1} << 11)) & ~((uint64_t{1} << 12) - 1);  // round away 12 bits
        }
        acc += XXH3_64bits(key, d * 8);
        asm volatile("" : "+r"(acc) : : "memory");
      } else {
        acc += XXH3_64bits(x, d * 8);
        asm volatile("" : "+r"(acc) : : "memory");
      }
    }
  auto t1 = std::chrono::steady_clock::now();
  sink = acc;
  return std::chrono::duration<double, std::nano>(t1 - t0).count() / (double(n) * reps);
}
int main() {
  for (size_t d : {2, 5, 10, 20}) {
    double a = 1e9, b = 1e9;
    for (int k = 0; k < 5; k++) { a = std::min(a, bench<false>(d)); b = std::min(b, bench<true>(d)); }
    std::printf("d=%2zu exact %.2f ns  quantized %.2f ns  (+%.2f)\n", d, a, b, b - a);
  }
}
