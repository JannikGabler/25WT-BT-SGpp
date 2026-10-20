/**
 * TODO: Document
 */
#include <cstddef>
#include <iostream>
#include <ostream>
#include "template_complete_sg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:
MATLAB SG Kit:
*/

static constexpr size_t nDim = 8;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 2;
static constexpr size_t nRuns = 5;

int main() {
  std::cout << "Warning (!): This benchmark might take some time.\n" << std::endl;

  quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns);
}