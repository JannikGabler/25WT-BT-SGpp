// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cross_call_operator_data_descriptor.hpp
 * @brief Defines @c CrossCallOperatorDataDescriptor, how an operator names an entry of the
 * cross-call operator data cache.
 */
#pragma once

#include <string_view>

namespace sgpp {
namespace combigrid {

namespace misc {

/**
 * @brief Names an entry of a @c CrossCallOperatorDataCache: together with the type of the data,
 * @ref key identifies the entry; @ref operatorName and @ref description are shown to the user
 * (@c CrossCallOperatorDataInfo).
 *
 * The views must stay valid during the cache call (operators pass string literals); the cache
 * copies them when it creates the entry.
 */
struct CrossCallOperatorDataDescriptor {
  std::string_view operatorName;  ///< The operator and grid type, e.g. "quadrature (SparseGrid)".
  std::string_view description;   ///< What the data is.
  std::string_view key;  ///< Reuse conditions beyond the grid and the type ("" if there are none).
};

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
