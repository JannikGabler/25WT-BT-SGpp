// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_generation_instruction.hpp
 * @brief Abstract base class for sparse-grid generation instructions.
 *
 * A "generation instruction" carries everything needed to materialize a
 * combination-technique sparse grid: its setup (@c SparseGridSetup: the spatial
 * domain, the per-dimension node-generation rules and growth functions, the
 * boundary level offset) and the algorithmic recipe that produces the level
 * multi-indices and their combination coefficients.
 */
#pragma once

#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Abstract base class for combination-technique sparse-grid
 * generators.
 *
 * Concrete subclasses (e.g. @c CompleteSGGenInstr,
 * @c MIVecSGGenInstr) implement @ref genReducedMIVec and
 * @ref genReducedMIVecWithCoeffs to produce the level multi-indices and the
 * matching combination coefficients of a specific combination scheme. The
 * setup getters and setters are inherited from @c SparseGridSetup; a grid
 * built from the instruction stores a copy of the setup only.
 */
class SGGenInstr : public SparseGridSetup {
 public:
  /***********
  Constructors
  ***********/
  /**
   * @brief Constructs a generator for an @p nDim -dimensional sparse grid with the default
   * setup (see @c SparseGridSetup).
   *
   * @param nDim Spatial dimensionality.
   */
  SGGenInstr(const size_t nDim) : SparseGridSetup(nDim) {};

  /**********
  Destructors
  **********/
  virtual ~SGGenInstr() = default;

  /*********************
  Sparse grid generation
  *********************/
  /**
   * TODO: Document
   * Includes all multi-indices (even those with a CT coefficient of 0).
   */
  virtual LvlMIVec genFullMIVec() const = 0;

  /**
   * TODO: Document
   * Differs from genFullMIVec by only including multi-indices with a CT coefficient unequal to 0.
   */
  virtual LvlMIVec genReducedMIVec() const = 0;

  /**
   * TODO: Document
   * Includes all multi-indices (even those with a CT coefficient of 0).
   */
  virtual std::pair<LvlMIVec, std::vector<CTCoeffType>> genFullMIVecWithCoeffs() const = 0;

  /**
   * TODO: Document
   * Differs from genFullMIVecWithCoeffs by only including multi-indices with a CT coefficient
   * unequal to 0.
   */
  virtual std::pair<LvlMIVec, std::vector<CTCoeffType>> genReducedMIVecWithCoeffs() const = 0;
};

}  // namespace combigrid
}  // namespace sgpp
