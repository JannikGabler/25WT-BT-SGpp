#include <chrono>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

constexpr std::size_t N = 100000000;
constexpr int REPEAT = 10;

// verhindert Wegoptimieren
volatile std::size_t sink;

/* ------------------ Fall 1 ------------------ */
std::size_t product_size_t(const std::vector<std::size_t>& v) {
  std::size_t result = 1;
  for (std::size_t x : v) result *= x;
  return result;
}

/* ------------------ Fall 2 ------------------ */
std::size_t product_uint(const std::vector<unsigned int>& v) {
  std::size_t result = 1;
  for (unsigned int x : v) result *= static_cast<std::size_t>(x);
  return result;
}

/* ------------------ Timing ------------------ */
template <typename Func>
long long benchmark(Func f) {
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < REPEAT; ++i) sink ^= f();
  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
  std::vector<std::size_t> vec_size_t(N);
  std::vector<unsigned int> vec_uint(N);

  // Kleine Werte, um Overflow zu vermeiden
  for (std::size_t i = 0; i < N; ++i) {
    vec_size_t[i] = (i % 5) + 1;
    vec_uint[i] = static_cast<unsigned int>((i % 5) + 1);
  }

  auto t1 = benchmark([&]() { return product_size_t(vec_size_t); });
  auto t2 = benchmark([&]() { return product_uint(vec_uint); });

  std::cout << "vector<size_t>       : " << t1 << " ms\n";
  std::cout << "vector<unsigned int> : " << t2 << " ms\n";

  // verhindert Optimierung
  std::cout << "sink = " << sink << "\n";
}
