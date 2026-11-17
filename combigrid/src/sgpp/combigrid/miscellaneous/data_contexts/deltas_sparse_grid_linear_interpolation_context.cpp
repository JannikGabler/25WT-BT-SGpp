// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_linear_interpolation_index.hpp>
#include <span>

namespace sgpp {
namespace combigrid {
namespace misc {

DSGLinInterpContext::DSGLinInterpContext(const DSGLinInterpIndex& index)
    : cornerIds(2 * index.nRows()) {
  const size_t nRowValues = 2 * index.nRows();
  doubles = std::make_unique_for_overwrite<double[]>(2 * nRowValues);
  factors = std::span<double>(doubles.get(), nRowValues);
  coords = std::span<double>(doubles.get() + nRowValues, nRowValues);
}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
