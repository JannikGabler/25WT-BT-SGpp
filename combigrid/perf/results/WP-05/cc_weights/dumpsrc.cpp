#include <cstdio>
#include <vector>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/default_rules/clenshaw_curtis_quadrature_rule.hpp>
int main(int, char** argv) {
  FILE* f = std::fopen(argv[1], "wb"); sgpp::combigrid::quadrature_rules::ClenshawCurtisQuadRule r;
  for (size_t l = 0; l <= LMAX; l++) { size_t N = (size_t(2) << l) + 1; std::vector<double> w(N);
    r.genWeightsInplace(w); std::fwrite(w.data(), 8, N, f); }
  std::fclose(f);
}
