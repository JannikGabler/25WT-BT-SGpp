#include <cstdio>
#include <vector>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
int main(int, char** argv) {
  FILE* f = std::fopen(argv[1], "wb");
  for (size_t l = 0; l <= 20; l++) { size_t N = (size_t(2) << l) + 1; std::vector<double> w(N);
    sgpp::combigrid::getClenshawCurtisQuadRule()->genWeightsInplace(w); std::fwrite(w.data(), 8, N, f); }
  std::fclose(f);
}
