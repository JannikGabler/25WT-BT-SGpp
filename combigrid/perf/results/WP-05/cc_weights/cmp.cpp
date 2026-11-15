#include <bit>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <vector>
#include "old_cc.hpp"
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
int main() {
  sgpp::combigrid::quadrature_rules_old::OldCC o; sgpp::combigrid::quadrature_rules::ClenshawCurtisQuadRule n;
  for (size_t l = 0; l <= 20; l++) { size_t N = (size_t(2) << l) + 1;
    std::vector<double> a(N), b(N); o.genWeightsInplace(a); n.genWeightsInplace(b);
    long maxu = 0; size_t nd = 0;
    for (size_t i = 0; i < N; i++) { long d = std::labs((long)std::bit_cast<int64_t>(a[i]) - (long)std::bit_cast<int64_t>(b[i])); if (d) nd++; if (d > maxu) maxu = d; }
    std::printf("n=%zu ndiff=%zu maxulp=%ld\n", N, nd, maxu); }
}
