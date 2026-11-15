#include <bit>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <vector>
int main() {
  const auto* rule = sgpp::combigrid::getClenshawCurtisQuadRule();
  uint64_t total = 0xcbf29ce484222325ULL;
  for (size_t l = 0; l <= 20; l++) for (size_t extra : {0, 2}) { if (extra == 0 && l > 10) continue;
    size_t n = (size_t(2) << l) - 1 + extra;
    std::vector<double> w(n);
    auto t0 = std::chrono::steady_clock::now();
    rule->genWeightsInplace(w);
    auto t1 = std::chrono::steady_clock::now();
    uint64_t h = 0xcbf29ce484222325ULL;
    for (double x : w) h = (h ^ std::bit_cast<uint64_t>(x)) * 0x100000001b3ULL;
    total = (total ^ h) * 0x100000001b3ULL;
    if (l >= 14) std::printf("n=%zu hash=%016llx time=%.2f ms\n", n, (unsigned long long)h, std::chrono::duration<double, std::milli>(t1 - t0).count());
  }
  std::printf("total=%016llx\n", (unsigned long long)total);
}
