// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_tools.hpp
 * @brief How operators obtain their query-independent data: from the grid's cross-call operator
 * data cache or built for one call, as their @c useCrossCallOperatorDataCache option says.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/grids/sparse_grid_base.hpp>
#include <sgpp/combigrid/miscellaneous/cross_call_operator_data_cache/cross_call_operator_data_descriptor.hpp>
#include <utility>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace cross_operator_call {

/// @brief Counts one build of operator data (see @ref nOperatorDataBuilds;
/// diagnostics for tests: whether a call built or reused its data).
void countOperatorDataBuild() noexcept;

/**
 * @brief Returns the number of builds of operator data by @ref getOperatorData in this process
 * (with and without the cache; diagnostics for tests: whether a call built or reused its data).
 */
size_t nOperatorDataBuilds() noexcept;

/**
 * @brief Returns the operator data of type @p T of @p grid: with @p useCache, from the grid's
 * cross-call operator data cache (built by @p build and stored if not present), else built by
 * @p build for the caller only (the cache is not touched).
 *
 * @tparam T         Type of the data.
 * @tparam Build     Callable without arguments returning a @c std::unique_ptr<T>.
 * @param grid       Grid of the operator call.
 * @param useCache   The operator's @c useCrossCallOperatorDataCache option.
 * @param descriptor Key and description of the cache entry.
 * @param build      Builds the data.
 * @return The data; the caller keeps it alive for the duration of its call.
 */
template <typename T, typename Build>
std::shared_ptr<const T> getOperatorData(const SparseGridBase& grid, const bool useCache,
                                         const misc::CrossCallOperatorDataDescriptor& descriptor,
                                         Build&& build) {
  const auto countedBuild = [&]() {
    countOperatorDataBuild();
    return std::forward<Build>(build)();
  };
  if (!useCache) return std::shared_ptr<const T>(countedBuild());
  return grid.getCrossCallOperatorDataCache().getOrBuild<T>(descriptor, countedBuild);
}

}  // namespace cross_operator_call
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
