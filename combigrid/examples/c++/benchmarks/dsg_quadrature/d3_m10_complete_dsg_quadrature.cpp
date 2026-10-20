/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:   0.002325 s
*/

static constexpr size_t nDim = 3;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 100;
static constexpr size_t nRuns = 500;

int main() { dsg_quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns); }