// Compares A's tools::compensated{Sum,DotProduct,DotProducts} (checkpoint 809b8cfe1) with B's
// tools::numerics:: versions bitwise on random ill-conditioned inputs (CT-like alternating integer
// coefficients, random doubles, exact cancellations, special values).
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <random>
#include <span>
#include <vector>

#include <sgpp/combigrid/tools/math/compensated_sum.hpp>       // A (from the scratch include dir)
#include <sgpp/combigrid/tools/numerics/compensated_sum_tools.hpp>  // B (from the repo)

namespace A = sgpp::combigrid::tools;
namespace B = sgpp::combigrid::tools::numerics;
using Coeff = int64_t;

static uint64_t bits(double x) { return std::bit_cast<uint64_t>(x); }

int main() {
  std::mt19937_64 rng(12345);
  size_t nChecks = 0, nDiff = 0;
  auto check = [&](double a, double b, const char* what, size_t n) {
    nChecks++;
    if (bits(a) != bits(b)) {
      if (nDiff < 20) std::printf("DIFF %s n=%zu: %.17g vs %.17g\n", what, n, a, b);
      nDiff++;
    }
  };
  const std::vector<size_t> sizes{0, 1, 2, 3, 7, 16, 100, 1001, 10626, 54264};
  for (int rep = 0; rep < 40; rep++) {
    for (const size_t n : sizes) {
      std::uniform_int_distribution<Coeff> cd(-200000, 200000);
      std::uniform_real_distribution<double> vd(-1.0, 1.0);
      std::uniform_int_distribution<int> ed(-60, 60);
      std::vector<Coeff> c(n);
      std::vector<double> v(n);
      for (size_t i = 0; i < n; i++) {
        c[i] = (rep % 3 == 0) ? ((i % 2) ? -1 : 1) * Coeff(i + 1) : cd(rng);
        v[i] = (rep % 4 == 1) ? std::ldexp(vd(rng), ed(rng)) : 0.5 + 1e-3 * vd(rng);
      }
      if (rep == 5 && n >= 3) { v[0] = 1e100; v[1] = 1.0; v[2] = -1e100; c[0] = c[1] = c[2] = 1; }
      check(A::compensatedSum(v), B::compensatedSum<double, double>(v), "sum", n);
      check(A::compensatedSum(c), B::compensatedSum<double, Coeff>(c), "sum-int", n);
      check(A::compensatedDotProduct(c, v), B::compensatedDotProduct<double, Coeff, double>(c, v),
            "dot", n);
      check(A::compensatedDotProduct(v, c), B::compensatedDotProduct<double, double, Coeff>(v, c),
            "dot-swapped", n);
      std::vector<float> cf(c.begin(), c.end());
      check(A::compensatedDotProduct(cf, v), B::compensatedDotProduct<double, float, double>(cf, v),
            "dot-float", n);
      for (const size_t m : {1, 2, 3, 5}) {
        std::vector<double> b(n * m);
        for (auto& x : b) x = (rep % 4 == 1) ? std::ldexp(vd(rng), ed(rng)) : vd(rng);
        std::vector<double> ra(m, -1), rb(m, -2);
        A::compensatedDotProducts(c, b, std::span<double>(ra));
        B::compensatedDotProducts<double, Coeff, double>(c, b, std::span<double>(rb));
        for (size_t k = 0; k < m; k++) check(ra[k], rb[k], "dots", n);
      }
      // Accumulator
      A::CompensatedSum sa;
      B::CompensatedSum<double> sb;
      for (size_t i = 0; i < n; i++) {
        sa.add(v[i]); sb.add(v[i]);
        sa.addProduct(double(c[i]), v[i]); sb.addProduct(double(c[i]), v[i]);
      }
      check(sa.value(), sb.value(), "accumulator", n);
    }
  }
  std::printf("compensated sum A vs B: %zu checks, %zu bitwise differences\n", nChecks, nDiff);
  return nDiff != 0;
}
