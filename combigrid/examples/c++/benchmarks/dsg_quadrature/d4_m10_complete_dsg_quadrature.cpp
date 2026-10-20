/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:   0.019775 s
*/

static constexpr size_t nDim = 4;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 30;
static constexpr size_t nRuns = 150;

int main() { dsg_quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns); }