/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:   5.641016 s
*/

static constexpr size_t nDim = 7;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 2;
static constexpr size_t nRuns = 5;

int main() { dsg_quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns); }