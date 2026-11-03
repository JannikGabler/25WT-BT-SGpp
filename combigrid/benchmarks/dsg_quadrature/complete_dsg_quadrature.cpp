/**
 * TODO: Document
 */
#include <cstddef>
#include "template_complete_dsg_quadrature.hpp"

using namespace sgpp::combigrid::benchmarks;

int main(int argc, char* argv[]) {
  if (argc != 7) {
    std::cerr << "Invalid number of arguments! Received " << (argc - 1) << " but requires 6."
              << std::endl;
    return 1;
  }

  size_t minWarmupRounds = 0;
  size_t minRounds = 0;
  double minWarmupTime = 0.0;
  double minRunTime = 0.0;
  size_t nDim = 0;
  size_t maxLvl = 0;

  try {
    minWarmupRounds = std::stoull(argv[1]);
    minRounds = std::stoull(argv[2]);
    minWarmupTime = std::stod(argv[3]);
    minRunTime = std::stod(argv[4]);
    nDim = std::stoull(argv[5]);
    maxLvl = std::stoull(argv[6]);
  } catch (const std::exception& e) {
    std::cerr << "Error while parsing arguments: " << e.what() << "\n";
    return 1;
  }

  dsg_quadrature_benchmark::startBenchmark(nDim, maxLvl, minWarmupRounds, minRounds, minWarmupTime,
                                           minRunTime);

  return 0;
}