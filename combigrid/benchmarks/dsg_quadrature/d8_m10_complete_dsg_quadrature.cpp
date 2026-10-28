/**
 * TODO: Document
 */
#include <cstddef>
#include <iostream>
#include <ostream>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:   33.297704 s
*/

static constexpr size_t nDim = 8;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 2;
static constexpr size_t nRuns = 5;

int main() {
  std::cout << "Warning (!): This benchmark might take some time.\n" << std::endl;

  dsg_quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns);
}