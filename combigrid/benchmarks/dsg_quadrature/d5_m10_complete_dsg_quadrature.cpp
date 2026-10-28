/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

/*
This:   0.130842 s
MATLAB SG Kit:
*/

static constexpr size_t nDim = 5;
static constexpr size_t maxLvl = 10;
static constexpr size_t nWarmupRuns = 15;
static constexpr size_t nRuns = 30;

int main() { dsg_quadrature_benchmark::run<maxLvl, nDim>(nWarmupRuns, nRuns); }