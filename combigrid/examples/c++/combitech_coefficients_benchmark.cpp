#include <omp.h>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <random>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>
#include "sgpp/combigrid/type_defs.hpp"

// ==========================================================
// Compile-time parameters
// ==========================================================
constexpr size_t N_DIM = 6;   // number of dimensions
constexpr size_t N_MI = 100;  // number of multi-indices
constexpr size_t N_RUNS = 10;

using namespace sgpp::combigrid;

size_t getPeakRSS_KB() {
  std::ifstream f("/proc/self/status");
  std::string line;
  while (std::getline(f, line)) {
    if (line.rfind("VmHWM:", 0) == 0) {
      size_t kb = 0;
      std::sscanf(line.c_str(), "VmHWM: %zu kB", &kb);
      return kb;
    }
  }
  return 0;
}

// ==========================================================
// Variant A: original (explicit assignment)
// ==========================================================
std::vector<int> compute_A(const LvlMIVec& miVec) {
  const auto lookup = miVec.lookup();
  const size_t nMI = miVec.nMI();

  std::vector<int> current(nMI, 1);
  std::vector<int> next(nMI, 0);

  for (size_t dim = 0; dim < miVec.nDim(); ++dim) {
#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < nMI; ++i) {
      LvlMI succ = miVec[i];
      succ[dim]++;
      size_t s = lookup->find(succ);

      if (s < nMI)
        next[i] = current[i] - current[s];
      else
        next[i] = current[i];
    }
    current.swap(next);
  }
  return current;
}

// ==========================================================
// Variant B: incremental (corrected version)
// ==========================================================
std::vector<int> compute_B(const LvlMIVec& miVec) {
  const auto lookup = miVec.lookup();
  const size_t nMI = miVec.nMI();

  std::vector<int> current(nMI, 1);
  std::vector<int> next(nMI);

  for (size_t dim = 0; dim < miVec.nDim(); ++dim) {
    std::memcpy(next.data(), current.data(), nMI * sizeof(int));

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < nMI; ++i) {
      LvlMI succ = miVec[i];
      succ[dim]++;
      size_t s = lookup->find(succ);

      if (s < nMI) next[i] -= current[s];
    }
    current.swap(next);
  }
  return current;
}

// ==========================================================
// Variant C: HPC-optimized (precompute succIdx + memcpy)
// ==========================================================
std::vector<int> compute_C(const LvlMIVec& miVec) {
  const auto lookup = miVec.lookup();
  const size_t nMI = miVec.nMI();

  std::vector<int> current(nMI, 1);
  std::vector<int> next(nMI);
  std::vector<size_t> succIdx(nMI);

  for (size_t dim = 0; dim < miVec.nDim(); ++dim) {
#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < nMI; ++i) {
      LvlMI succ = miVec[i];
      succ[dim]++;
      succIdx[i] = lookup->find(succ);
    }

    std::memcpy(next.data(), current.data(), nMI * sizeof(int));

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < nMI; ++i) {
      size_t s = succIdx[i];
      if (s < nMI) next[i] -= current[s];
    }
    current.swap(next);
  }
  return current;
}

// ==========================================================
// Timing helper
// ==========================================================
template <typename F>
double time_it(F&& f) {
  auto t0 = std::chrono::high_resolution_clock::now();
  volatile auto r = f();  // prevent dead-code elimination
  auto t1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dt = t1 - t0;
  return dt.count();
}

// ==========================================================
// Main
// ==========================================================
int main() {
  // deterministic synthetic data
  std::random_device rd;
  const auto seed = rd();
  std::mt19937 gen(seed);
  std::uniform_int_distribution<LvlType> valDist(0, 10);

  std::vector<std::vector<LvlType>> miVec(N_MI, std::vector<LvlType>(N_DIM));
  for (size_t i = 0; i < N_MI; ++i) {
    for (size_t d = 0; d < N_DIM; ++d) {
      miVec[i][d] = valDist(gen);
    }
  }

  const LvlMIVec vec(miVec);

  std::cout << "Constructing closure..." << std::endl;
  const LvlMIVec closure = vec.downwardsClosure();
  std::cout << "Constructed closure." << std::endl;

  if (!closure.isDownwardsClosed()) {
    std::cerr << "MIVec is not downwardsclosed!" << std::endl;
  }

  double tA = 0, tB = 0, tC = 0;

  for (size_t r = 0; r < N_RUNS; ++r) {
    tA += time_it([&] { return compute_A(closure); });
    tB += time_it([&] { return compute_B(closure); });
    tC += time_it([&] { return compute_C(closure); });
  }

  std::cout << "=== Benchmark results ===\n";
  std::cout << "Dims: " << N_DIM << ", MI: " << N_MI << "\n";
  std::cout << "Variant A (baseline): " << tA / N_RUNS << " s\n";
  std::cout << "Variant B (increment): " << tB / N_RUNS << " s\n";
  std::cout << "Variant C (HPC opt): " << tC / N_RUNS << " s\n";

  return 0;
}