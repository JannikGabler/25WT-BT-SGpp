// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file unit_node_table.hpp
 * @brief The node lists of a sparse grid on the unit interval, for the operators that work in
 * normalized coordinates (operator data of the grid).
 */
#pragma once

#include <sgpp/combigrid/grids/node_table.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

/**
 * @brief The node table of a sparse grid before the mapping onto the domain: the same layout as
 * the grid's @c NodeTable, with the unit nodes of the node-generation functions.
 *
 * The global and the linear interpolation compute the 1D basis values resp. the bracketing cell
 * of the query point in normalized coordinates (on @f$[0,1]@f$). They build this table on their
 * first call on a grid and keep it in the grid's operator data (@c misc::OperatorDataCache, see
 * @c tools::node_table_generation::getUnitNodeTable); the grid itself stores only the nodes on the
 * domain.
 */
struct UnitNodeTable {
  NodeTable table;  ///< Node lists on the unit interval.
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
