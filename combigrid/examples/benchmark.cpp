#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>

// --------------------------------------------------
// Test-Funktor (empfohlene Methode)
// --------------------------------------------------
struct SinCosFunctor {
  inline double operator()(double x, double y) const noexcept { return std::sin(x) * std::cos(y); }
};

// --------------------------------------------------
// Benchmark-Funktionen
// --------------------------------------------------
template <typename Func>
double benchmark_template(Func&& f, std::size_t N) {
  double sum = 0.0;
  for (std::size_t i = 0; i < N; ++i) {
    sum += f(0.001 * i, 0.002 * i);
  }
  return sum;
}

double benchmark_std_function(const std::function<double(double, double)>& f, std::size_t N) {
  double sum = 0.0;
  for (std::size_t i = 0; i < N; ++i) {
    sum += f(0.001 * i, 0.002 * i);
  }
  return sum;
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main() {
  constexpr std::size_t N = 50000000;

  // std::function
  std::function<double(double, double)> f_std = [](double x, double y) {
    return std::sin(x) * std::cos(y);
  };

  // Functor
  SinCosFunctor f_functor;

  // Warmup (verhindert Cold-Start-Effekte)
  benchmark_template(f_functor, 1000);
  benchmark_std_function(f_std, 1000);

  // ----------------------------------------------
  // std::function Benchmark
  // ----------------------------------------------
  auto t1 = std::chrono::high_resolution_clock::now();
  volatile double r1 = benchmark_std_function(f_std, N);
  auto t2 = std::chrono::high_resolution_clock::now();

  // ----------------------------------------------
  // Template/Functor Benchmark
  // ----------------------------------------------
  auto t3 = std::chrono::high_resolution_clock::now();
  volatile double r2 = benchmark_template(f_functor, N);
  auto t4 = std::chrono::high_resolution_clock::now();

  // Zeiten
  auto dt_std = std::chrono::duration<double>(t2 - t1).count();
  auto dt_tpl = std::chrono::duration<double>(t4 - t3).count();

  std::cout << "std::function   : " << dt_std << " s\n";
  std::cout << "template/functor: " << dt_tpl << " s\n";
  std::cout << "speedup         : " << dt_std / dt_tpl << "x\n";

  // Verhindert Optimierung
  std::cout << "dummy: " << r1 + r2 << "\n";

  return 0;
}
