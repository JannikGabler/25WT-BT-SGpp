// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// WP-14 follow-up: bitwise probe of the datadriven kernels whose type punning was replaced by
// std::bit_cast (OperationMultiEvalStreaming, OperationMultiEvalModMaskStreaming,
// OperationMultipleEvalSubspaceCombined). Runs mult and multTranspose on fixed random data and
// prints every result with %a, so the outputs of two library builds can be compared with diff.
// The SubspaceCombined kernel has no Boost test without OpenCL (its test needs USE_OCL == 1).
//
// Build against an all-module build (repo root, LD_LIBRARY_PATH=lib at run time):
//   g++ -std=c++23 -O2 -fopenmp -DZLIB=1 -Ibase/src -Idatadriven/src -Isolver/src
//       -Ioptimization/src -Ipde/src ddprobe.cpp -Llib -lsgppdatadriven -lsgppbase -o ddprobe

#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/operation/hash/OperationMultipleEval.hpp>
#include <sgpp/datadriven/DatadrivenOpFactory.hpp>
#include <sgpp/datadriven/operation/hash/DatadrivenOperationCommon.hpp>

#include <cstdio>
#include <memory>
#include <random>

using sgpp::datadriven::OperationMultipleEvalConfiguration;
using sgpp::datadriven::OperationMultipleEvalSubType;
using sgpp::datadriven::OperationMultipleEvalType;

int main() {
  struct Case {
    const char* name;
    bool modLinear;
    OperationMultipleEvalType type;
    OperationMultipleEvalSubType subType;
  };
  const Case cases[] = {
      {"streaming-linear", false, OperationMultipleEvalType::STREAMING,
       OperationMultipleEvalSubType::DEFAULT},
      {"streaming-modlinear(modmask)", true, OperationMultipleEvalType::STREAMING,
       OperationMultipleEvalSubType::DEFAULT},
      {"subspace-combined", false, OperationMultipleEvalType::SUBSPACELINEAR,
       OperationMultipleEvalSubType::COMBINED}};

  for (const size_t dim : {3, 5}) {
    for (const Case& c : cases) {
      std::unique_ptr<sgpp::base::Grid> grid(c.modLinear
                                                 ? sgpp::base::Grid::createModLinearGrid(dim)
                                                 : sgpp::base::Grid::createLinearGrid(dim));
      grid->getGenerator().regular(5);
      const size_t nGrid = grid->getSize();
      const size_t nData = 517;

      std::mt19937_64 rng(42 + dim);
      std::uniform_real_distribution<double> unit(0.0, 1.0);
      sgpp::base::DataMatrix dataset(nData, dim);
      for (size_t i = 0; i < nData; i++) {
        for (size_t d = 0; d < dim; d++) dataset.set(i, d, unit(rng));
      }
      sgpp::base::DataVector alpha(nGrid);
      for (size_t i = 0; i < nGrid; i++) alpha[i] = 2.0 * unit(rng) - 1.0;
      sgpp::base::DataVector source(nData);
      for (size_t i = 0; i < nData; i++) source[i] = 2.0 * unit(rng) - 1.0;

      OperationMultipleEvalConfiguration configuration(c.type, c.subType);
      std::unique_ptr<sgpp::base::OperationMultipleEval> op(
          sgpp::op_factory::createOperationMultipleEval(*grid, dataset, configuration));
      sgpp::base::DataVector result(nData);
      op->mult(alpha, result);
      sgpp::base::DataVector resultTranspose(nGrid);
      op->multTranspose(source, resultTranspose);

      std::printf("# %s d=%zu grid=%zu data=%zu\n", c.name, dim, nGrid, nData);
      for (size_t i = 0; i < nData; i++) std::printf("mult %zu %a\n", i, result[i]);
      for (size_t i = 0; i < nGrid; i++) std::printf("multT %zu %a\n", i, resultTranspose[i]);
    }
  }
  return 0;
}
