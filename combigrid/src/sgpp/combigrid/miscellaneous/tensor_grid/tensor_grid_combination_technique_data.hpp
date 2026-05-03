/**
 * @file tensor_grid_combination_technique_data.hpp
 * @brief Bundle of (level multi-index, combination coefficient, tensor grid)
 * for one summand of a combination-technique sparse grid.
 */
#pragma once

#include <sgpp/combigrid/grids/tensor_grid.hpp>

namespace sgpp {
namespace combigrid {

/**
 * @brief Per-tensor-grid metadata stored inside a @c SparseGrid.
 *
 * For a single summand
 * @f$c_{\vec{\ell}}\,f_{\vec{\ell}}@f$ of the combination technique, this
 * struct holds the level multi-index @f$\vec{\ell}@f$ ( @ref mi ), the
 * integer combination coefficient @f$c_{\vec{\ell}}@f$ ( @ref coefficient ),
 * and the @c TensorGrid that carries the actual nodes ( @ref tensorGrid ).
 */
struct TensorGridCTData {
  LvlMI mi;                 ///< Level multi-index of the tensor grid.
  CTCoeffType coefficient;  ///< Combination-technique coefficient.
  TensorGrid tensorGrid;    ///< Underlying anisotropic tensor grid.

  /**
   * @brief Element-wise equality test.
   * @param other Other data bundle.
   * @return @c true iff @p other has the same multi-index, coefficient,
   * and tensor grid.
   */
  bool operator==(const TensorGridCTData& other) const;
};

}  // namespace combigrid
}  // namespace sgpp
