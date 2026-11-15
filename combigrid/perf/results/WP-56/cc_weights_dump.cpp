// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-56: dumps the Clenshaw-Curtis weights of the library it runs against (genWeights) as raw
// doubles, one file per node count: <outdir>/w_<tag>_<n>.bin. Run once per snapshot
// (LD_LIBRARY_PATH=<snapshot>/lib), compared by cc_weights_accuracy.py.
// Build: g++ -O2 -std=c++23 -I<repo>/combigrid/src -I<repo>/base/src cc_weights_dump.cpp
//        -L<snapshot>/lib -lsgppcombigrid -lsgppbase -o cc_weights_dump
#include <cstdio>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <string>

int main(int argc, char** argv) {
  const std::string outdir = argv[1], tag = argv[2];
  sgpp::combigrid::QuadRule* const rule = sgpp::combigrid::getClenshawCurtisQuadRule();
  for (const unsigned n : {3u, 9u, 64u, 65u, 129u, 1025u, 4097u, 32769u, 131073u, 1048577u,
                           2097153u}) {
    const sgpp::base::DataVector w = rule->genWeights(n);
    const std::string path = outdir + "/w_" + tag + "_" + std::to_string(n) + ".bin";
    FILE* const f = std::fopen(path.c_str(), "wb");
    std::fwrite(w.data(), sizeof(double), w.size(), f);
    std::fclose(f);
  }
}
