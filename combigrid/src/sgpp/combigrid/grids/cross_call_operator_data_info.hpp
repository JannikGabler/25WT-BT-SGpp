// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_info.hpp
 * @brief Defines @c CrossCallOperatorDataInfo, the read-only description of one entry of a grid's
 * cross-call operator data cache.
 */
#pragma once

#include <cstddef>
#include <string>

namespace sgpp {
namespace combigrid {

/**
 * @brief Description of one entry of the cross-call operator data cache of a grid (see
 * @c SparseGridBase::getCrossCallOperatorDataCacheEntries).
 *
 * An entry is query-independent data that an operator built for the grid and keeps for its later
 * calls (e.g. the index of the multilinear interpolation on a deltas sparse grid). The data itself
 * is internal to the operators; this is a copy of its description.
 */
struct CrossCallOperatorDataInfo {
  /// The operator (and its variant) that built the data, e.g. "interpolateLinear
  /// (DeltasSparseGrid)".
  std::string operatorName;
  /// What the data is.
  std::string description;
  /// The conditions of its reuse beyond the grid (empty: the data depends on the grid only).
  std::string key;
  /// Heap and object memory of the data in bytes (approximate: the capacities of its containers).
  size_t bytes = 0;
};

}  // namespace combigrid
}  // namespace sgpp
