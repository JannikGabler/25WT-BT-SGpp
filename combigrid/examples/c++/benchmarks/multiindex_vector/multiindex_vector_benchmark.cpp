// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file multiindex_vector_benchmark.cpp
 * @brief Micro-benchmarks of the dominant @c MIVec operations.
 *
 * Measures element comparisons, element-wise iteration, the multi-index-set
 * tools (CT coefficients, downwards closure, Pareto maxima), sparse-grid
 * construction from a multi-index set, and sorting/partitioning.
 *
 * Compiling with @c -DMIVEC_BENCHMARK_LEGACY_API restricts the program to the
 * API of the pre-redesign @c MIVec (no iterators), so the same source can be
 * built against an older tree for before/after comparisons. In that mode,
 * sorting an @c MIVec is done the only way it was possible: by copying the
 * multi-indices into a @c std::vector<MI>, sorting, and copying them back.
 *
 * Usage: multiindex_vector_benchmark [repetitions]. Prints the median time
 * per repetition in milliseconds and a checksum per benchmark.
 */

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <random>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>
#ifndef MIVEC_BENCHMARK_LEGACY_API
#include <sgpp/combigrid/multiindices/multiindex_lexicographic_less.hpp>
#endif

using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;

namespace {

/// Lexicographic order on MI (available with both APIs).
struct LexLess {
  bool operator()(const LvlMI& a, const LvlMI& b) const {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
  }
};

/// Runs @p body @p reps times after one warm-up run; prints the median time and the checksum.
void run(const char* name, const size_t reps, const std::function<size_t()>& body) {
  size_t checksum = body();
  std::vector<double> times;
  for (size_t rep = 0; rep < reps; rep++) {
    const auto start = std::chrono::steady_clock::now();
    checksum += body();
    const auto stop = std::chrono::steady_clock::now();
    times.push_back(std::chrono::duration<double, std::milli>(stop - start).count());
  }
  std::sort(times.begin(), times.end());
  std::printf("%-40s %10.3f ms   (checksum %zu)\n", name, times[times.size() / 2], checksum);
}

LvlMIVec randomMIVec(const size_t nDim, const size_t nMI, const unsigned maxValue,
                     const unsigned seed) {
  std::mt19937 generator(seed);
  std::uniform_int_distribution<unsigned> dist(0, maxValue);
  std::vector<std::vector<LvlType>> raw(nMI, std::vector<LvlType>(nDim));
  for (std::vector<LvlType>& mi : raw) {
    for (LvlType& entry : mi) {
      entry = dist(generator);
    }
  }
  return LvlMIVec(raw);
}

/// Hyperbolic-cross-like downwards-closed set: all MIs with sum of entries <= maxSum.
LvlMIVec simplexSet(const size_t nDim, const unsigned maxSum) {
  std::vector<std::vector<LvlType>> raw;
  std::vector<LvlType> mi(nDim, 0);
  while (true) {
    raw.push_back(mi);
    size_t dim = 0;
    while (dim < nDim) {
      mi[dim]++;
      unsigned sum = 0;
      for (const LvlType entry : mi) sum += entry;
      if (sum <= maxSum) break;
      mi[dim] = 0;
      dim++;
    }
    if (dim == nDim) break;
  }
  return LvlMIVec(raw);
}

}  // namespace

int main(int argc, char** argv) {
  const size_t reps = argc > 1 ? static_cast<size_t>(std::atoi(argv[1])) : 11;

  // Comparison-heavy loops.
  {
    const LvlMIVec vec = randomMIVec(4, 2000, 3, 1);
    run("compare all pairs v[i] == v[j]", reps, [&vec]() {
      size_t equalPairs = 0;
      for (size_t i = 0; i < vec.nMI(); i++) {
        for (size_t j = 0; j < vec.nMI(); j++) {
          equalPairs += (vec[i] == vec[j]);
        }
      }
      return equalPairs;
    });

    const LvlMI bound{2, 2, 2, 2};
    run("compare v[i] <= MI (x200)", reps, [&vec, &bound]() {
      size_t dominated = 0;
      for (size_t rep = 0; rep < 200; rep++) {
        for (size_t i = 0; i < vec.nMI(); i++) {
          dominated += (vec[i] <= bound);
        }
      }
      return dominated;
    });
  }

  // Element-wise iteration.
  {
    const LvlMIVec vec = randomMIVec(6, 200000, 9, 2);
    run("iterate v[i].sumOfElems()", reps, [&vec]() {
      size_t sum = 0;
      for (size_t i = 0; i < vec.nMI(); i++) {
        sum += vec[i].sumOfElems();
      }
      return sum;
    });
#ifndef MIVEC_BENCHMARK_LEGACY_API
    run("iterate range-for (MIView)", reps, [&vec]() {
      size_t sum = 0;
      for (const sgpp::combigrid::MIView<LvlType> mi : vec) {
        sum += mi.sumOfElems();
      }
      return sum;
    });
#endif
  }

  // Multi-index-set tools.
  {
    const LvlMIVec simplex = simplexSet(5, 16);  // Downwards closed.
    std::printf("(simplex set: nDim=%zu nMI=%zu)\n", simplex.nDim(), simplex.nMI());
    run("computeCTCoeffs (simplex d=5)", reps, [&simplex]() {
      const std::vector<sgpp::combigrid::CTCoeffType> coeffs =
          sgpp::combigrid::tools::computeCTCoeffs(simplex);
      return static_cast<size_t>(std::count(coeffs.begin(), coeffs.end(), 0));
    });
    // Copies of an MIVec that never computed its lookup, so that every repetition builds it.
    const LvlMIVec simplexWithoutCache = simplexSet(5, 16);
    run("isDownwardsClosed incl. lookup (simplex)", reps, [&simplexWithoutCache]() {
      const LvlMIVec copy(simplexWithoutCache);
      return static_cast<size_t>(copy.isDownwardsClosed());
    });
    run("isDownwardsClosed, cached lookup (simplex)", reps, [&simplex]() {
      return static_cast<size_t>(simplex.isDownwardsClosed());
    });
    run("paretoMaxima DWC (simplex d=5)", reps, [&simplexWithoutCache]() {
      const LvlMIVec copy(simplexWithoutCache);
      return copy.paretoMaxima(true)->size();
    });

    const LvlMIVec sparseSet = randomMIVec(5, 60, 7, 3);
    run("downwardsClosure (60 MIs, d=5)", reps, [&sparseSet]() {
      const LvlMIVec copy(sparseSet);
      return copy.downwardsClosure().nMI();
    });
  }

  // Sparse-grid construction from a multi-index set (closure + coefficients + tensor grids).
  {
    const LvlMIVec input = randomMIVec(4, 25, 7, 4);
    run("SparseGrid(MIVecSGGenInstr) d=4", reps, [&input]() {
      sgpp::combigrid::MIVecSGGenInstr genInstr(input);
      genInstr.setNodeGenFunc(sgpp::combigrid::getClenshawCurtisNodeGenFunc());
      genInstr.setLvl2GPCntFunc(sgpp::combigrid::linearLvl2GPCntFunction);
      const sgpp::combigrid::SparseGrid sg(genInstr);
      return sg.nTG();
    });
  }

  // Sorting and partitioning. Every repetition sorts a fresh copy of the input; the copy costs
  // are measured separately.
  const size_t sortDims[] = {4, 16};
  for (const size_t nDim : sortDims) {
    std::printf("(sorting: nDim=%zu nMI=200000)\n", nDim);
    const LvlMIVec input = randomMIVec(nDim, 200000, 15, 5);
    std::vector<LvlMI> inputVector;
    for (size_t i = 0; i < input.nMI(); i++) {
      inputVector.push_back(input[i]);
    }

    run("std::vector<MI>: copy only", reps, [&inputVector]() {
      const std::vector<LvlMI> vec(inputVector);
      return vec.size();
    });
    run("MIVec: copy only", reps, [&input]() {
      const LvlMIVec vec(input);
      return vec.nMI();
    });

    run("std::vector<MI>: sort", reps, [&inputVector]() {
      std::vector<LvlMI> vec(inputVector);
      std::sort(vec.begin(), vec.end(), LexLess());
      return static_cast<size_t>(vec.front()[0]);
    });
    run("std::vector<MI>: stable_sort", reps, [&inputVector]() {
      std::vector<LvlMI> vec(inputVector);
      std::stable_sort(vec.begin(), vec.end(), LexLess());
      return static_cast<size_t>(vec.front()[0]);
    });
    run("std::vector<MI>: partition", reps, [&inputVector]() {
      std::vector<LvlMI> vec(inputVector);
      return static_cast<size_t>(std::partition(vec.begin(), vec.end(), [](const LvlMI& mi) {
                                   return mi[0] < 8;
                                 }) -
                                 vec.begin());
    });

#ifdef MIVEC_BENCHMARK_LEGACY_API
    run("MIVec: sort (copy to vector<MI> + back)", reps, [&input]() {
      LvlMIVec vec(input);
      std::vector<LvlMI> tmp;
      tmp.reserve(vec.nMI());
      for (size_t i = 0; i < vec.nMI(); i++) tmp.push_back(vec[i]);
      std::sort(tmp.begin(), tmp.end(), LexLess());
      for (size_t i = 0; i < vec.nMI(); i++) vec.setMI(i, tmp[i]);
      return static_cast<size_t>(vec(0, 0));
    });
#else
    run("MIVec: sort", reps, [&input]() {
      LvlMIVec vec(input);
      std::sort(vec.begin(), vec.end(), sgpp::combigrid::MILexLess());
      return static_cast<size_t>(vec(0, 0));
    });
    run("MIVec: stable_sort", reps, [&input]() {
      LvlMIVec vec(input);
      std::stable_sort(vec.begin(), vec.end(), sgpp::combigrid::MILexLess());
      return static_cast<size_t>(vec(0, 0));
    });
    run("MIVec: partition", reps, [&input]() {
      LvlMIVec vec(input);
      return static_cast<size_t>(
          std::partition(vec.begin(), vec.end(),
                         [](const sgpp::combigrid::MIView<LvlType> mi) { return mi[0] < 8; }) -
          vec.begin());
    });
    run("MIVec: remove_if + erase", reps, [&input]() {
      LvlMIVec vec(input);
      vec.erase(std::remove_if(vec.begin(), vec.end(),
                               [](const sgpp::combigrid::MIView<LvlType> mi) { return mi[0] < 8; }),
                vec.end());
      return vec.nMI();
    });
#endif
  }

  return 0;
}
