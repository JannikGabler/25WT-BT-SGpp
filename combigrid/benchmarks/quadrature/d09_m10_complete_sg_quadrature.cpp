/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_sg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:
MATLAB SG Kit:
*/

static constexpr size_t nDim = 9;
static constexpr size_t maxLvl = 10;
static constexpr size_t minWarmupRounds = 2;
static constexpr size_t minRounds = 5;
static constexpr double minWarmupTime = 5.0;
static constexpr double minRunTime = 30.0;

int main() {
  std::cout << "This might take a while." << std::endl;

  quadrature_benchmark::startBenchmark(nDim, maxLvl, minWarmupRounds, minRounds, minWarmupTime,
                                       minRunTime);
}