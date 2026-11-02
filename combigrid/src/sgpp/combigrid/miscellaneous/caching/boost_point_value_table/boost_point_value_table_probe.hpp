// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file boost_point_value_table_probe.hpp
 * @brief Key of the heterogeneous lookups in the index of a @c misc::BoostPointValueTable.
 */
#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace boost_point_value_table {

/// @brief Key of the heterogeneous lookups in the index: a point and its hash.
struct Probe {
  const double* point;  ///< Coordinates of the point.
  uint64_t hash;        ///< @c BoostPointValueTable::hash of the point.
};

}  // namespace boost_point_value_table
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
