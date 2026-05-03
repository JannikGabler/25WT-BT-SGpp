/**
 * @file level_to_grid_point_count_functions.hpp
 * @brief Built-in level-to-grid-point-count growth functions.
 *
 * A level-to-count function maps a 1D refinement level @f$\ell@f$ to the
 * number of grid points @f$n_\ell@f$ generated at that level. The choice
 * of growth function controls the trade-off between accuracy and the size
 * of the resulting tensor / sparse grid.
 */
#ifndef COMBIGRID_LEVEL_TO_GRID_POINTS_COUNT_FUNCTIONS_HPP
#define COMBIGRID_LEVEL_TO_GRID_POINTS_COUNT_FUNCTIONS_HPP

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Linear growth: @f$n_\ell = \ell@f$ (one node per level increase).
 * @param lvl Refinement level.
 * @return Number of grid points at level @p lvl.
 */
GPCntType linearLvl2GPCntFunction(LvlType lvl);

/**
 * @brief Doubling growth: @f$n_\ell = 2^{\ell-1} + 1@f$ for @f$\ell\ge 1@f$
 * (the standard nested Clenshaw-Curtis growth rule).
 * @param lvl Refinement level.
 * @return Number of grid points at level @p lvl.
 */
GPCntType doublingLvl2GPCntFunction(LvlType lvl);

}  // namespace combigrid
}  // namespace sgpp

#endif
