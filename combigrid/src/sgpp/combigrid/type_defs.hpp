/**
 * @file type_defs.hpp
 * @brief Central type aliases used across the @c combigrid module.
 *
 * The integer types used to store levels, grid-point counts, and combination
 * coefficients are configurable here. Larger types should be picked when
 * working with higher levels, larger combination coefficients, or larger
 * numbers of grid points; smaller types reduce memory pressure and may
 * improve performance.
 */
#pragma once

#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/hashing/std_pair_hash.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

class NodeGenFunc;  ///< Forward declaration to avoid circular includes.

/******************************************************************************************
-Variable types-
Can be changed to better suit specific needs.
Larger data types are necessary when dealing with higher levels, coefficients, or number of
gridpoints. Smaller data types may increase performance.
******************************************************************************************/

/**
 * @brief Scalar type used to represent a refinement level in a single
 * dimension.
 */
using LvlType = unsigned int;

/**
 * @brief Multi-index of refinement levels, one entry per dimension
 * (level vector @f$\vec{\ell}@f$).
 */
using LvlMI = MI<LvlType>;

/**
 * @brief Vector of level multi-indices, used to enumerate the tensor grids
 * that participate in a combination-technique sparse grid.
 */
using LvlMIVec = MIVec<LvlType>;

/**
 * @brief Hash-based lookup structure mapping level multi-indices to their
 * position inside an LvlMIVec.
 */
using LvlMIVecLookup = misc::MIVecLookup<LvlType>;

/**
 * @brief Scalar type used to represent counts or per-dimension indices of
 * grid points.
 */
using GPCntType = unsigned int;

/**
 * @brief Multi-index of grid-point indices/counts, one entry per dimension.
 */
using GPMI = MI<GPCntType>;

/**
 * @brief Scalar type used to store combination-technique coefficients
 * (the integer weights @f$c_{\vec{\ell}}@f$ multiplying each tensor grid in
 * the combination sum).
 */
using CTCoeffType = int;

/*********************
-Fixed types-
Should not be changed.
*********************/

/**
 * @brief Axis-aligned hyper-rectangular domain, given as a vector of
 * @c (min, max) intervals (one entry per dimension).
 */
using HyperCubeArea = std::vector<std::pair<double, double>>;
// using SourceFunc =
// std::function<double(const base::DataVector&)>;  // double (*)(const sgpp::base::DataVector&);

// using NodeGenFunc = sgpp::base::DataVector (*)(GPCntType);

/**
 * @brief Function pointer mapping a refinement level to the number of grid
 * points generated at that level (e.g. linear or doubling growth).
 */
using Lvl2GPCntFunc = GPCntType (*)(LvlType);
// using NodeGenFuncId = uint64_t;

/**
 * @brief Cache mapping a (node-generation function, requested point count)
 * pair to the corresponding 1D node positions.
 *
 * Used to avoid recomputing node positions for tensor grids that share a
 * node-generation function and grid-point count along some dimension.
 */
using SGGenNodeLookup = std::unordered_map<std::pair<NodeGenFunc*, GPCntType>, base::DataVector,
                                           misc::PairHash<NodeGenFunc*, GPCntType>>;

/**
 * @brief Identifier type for cache entries.
 */
using CacheId = uint64_t;

}  // namespace combigrid
}  // namespace sgpp
