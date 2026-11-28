// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sparse_grid_setup.hpp
 * @brief The setup of a sparse grid: domain, 1D node families, growth functions, boundary
 * level offset and node storage.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <sgpp/combigrid/node_families/node_family.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

// Forward declaration
class NodeStorage;

/**
 * @brief Everything that defines a sparse grid apart from its tensor grids: the domain, the 1D
 * node family and the level-to-grid-point-count (growth) function of every dimension and the
 * boundary level offset; plus the @c NodeStorage the grid takes its 1D node lists from.
 *
 * Generation instructions (@c SGGenInstr) derive from this class and add the recipe for the level
 * multi-indices; a grid stores a copy of the setup of the instruction it was built from
 * (@c SparseGridBase::getSetup) and does not refer to the instruction afterwards. A value type
 * (copyable; compared member-wise except for the node storage, which does not change the nodes).
 *
 * Default state: domain @f$[0,1]^{\mathrm{nDim}}@f$, equidistant nodes, doubling growth
 * (@c doublingLvl2GPCntFunction), boundary level offset @c 0, the global node storage
 * (@c NodeStorage::global).
 */
class SparseGridSetup {
 public:
  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the default setup of an @p nDim -dimensional sparse grid.
   * @param nDim Spatial dimensionality.
   */
  explicit SparseGridSetup(size_t nDim);

  /// @}

  /// @name Getters
  /// @{

  /// @brief Returns the spatial dimensionality.
  size_t nDim() const;

  /**
   * @brief Returns the boundary level offset.
   *
   * Tensor grids whose level in some dimension is @c >= the offset use boundary nodes in that
   * dimension. A value of @c 0 means every level uses the boundary; a very large value disables
   * boundary use altogether.
   */
  LvlType getBoundaryLevelOffset() const;

  /// @brief Returns the per-dimension domain (one @c (min, max) interval per dimension).
  const HyperCubeArea& getDomain() const;

  /// @brief Returns the @c (min, max) interval of dimension @p dim.
  std::pair<double, double> getDomainForDim(size_t dim) const;

  /// @brief Returns the per-dimension node families.
  const std::vector<NodeFamily*>& getNodeFamilies() const;

  /// @brief Returns the node family of dimension @p dim.
  NodeFamily* getNodeFamilyForDim(size_t dim) const;

  /// @brief Returns the per-dimension level-to-grid-point-count growth functions.
  const std::vector<Lvl2GPCntFunc>& getLvl2GPCntFuncs() const;

  /// @brief Returns the level-to-grid-point-count growth function of dimension @p dim.
  Lvl2GPCntFunc getLvl2GPCntFuncForDim(size_t dim) const;

  /// @brief Returns the storage the grid takes its 1D node lists from.
  const std::shared_ptr<NodeStorage>& getNodeStorage() const;

  /// @}

  /// @name Setters
  /// @{

  /// @brief Sets the boundary level offset (see @ref getBoundaryLevelOffset).
  void setBoundaryLevelOffset(LvlType boundaryLevelOffset);

  /// @brief Replaces the interval of dimension @p dim.
  void setDomainForDim(std::pair<double, double> interval, size_t dim);
  /// @brief Sets every dimension to the same @p interval.
  void setDomain(std::pair<double, double> interval);
  /// @brief Replaces the domain (at most @ref nDim intervals are copied).
  void setDomain(const HyperCubeArea& domain);

  /// @brief Sets the node family of dimension @p dim.
  void setNodeFamilyForDim(NodeFamily* nodeFamily, size_t dim);
  /// @brief Uses the same node family in every dimension.
  void setNodeFamily(NodeFamily* nodeFamily);
  /// @brief Replaces the per-dimension node families (at most @ref nDim are copied).
  void setNodeFamilies(const std::vector<NodeFamily*>& nodeFamilies);

  /// @brief Sets the growth function of dimension @p dim.
  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);
  /// @brief Uses the same growth function in every dimension.
  void setLvl2GPCntFunc(Lvl2GPCntFunc lvl2GPCntFunc);
  /// @brief Replaces the per-dimension growth functions (at most @ref nDim are copied).
  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs);

  /**
   * @brief Sets the storage the grid takes its 1D node lists from (default:
   * @c NodeStorage::global; another storage keeps the grids' node lists apart from those of other
   * grids).
   * @throws std::invalid_argument if @p nodeStorage is null.
   */
  void setNodeStorage(std::shared_ptr<NodeStorage> nodeStorage);

  /// @}

  /// @name Helper operations
  /// @{

  /**
   * @brief Resizes the setup to a different dimensionality.
   *
   * New dimensions get default-constructed entries (empty interval, no node family,
   * no growth function); surplus dimensions are dropped.
   *
   * @param newDimCnt New dimensionality.
   */
  void resize(size_t newDimCnt);

  /// @brief Returns the volume of the domain (@c 0 for a 0-dimensional setup).
  double getVolumeOfDomain() const;

  /// @brief Returns the distinct node families of all dimensions.
  std::vector<NodeFamily*> getUniqueNodeFamilies() const;

  /**
   * @brief Returns whether the nodes are nested (see @c NodeFamily::isNested) in every
   * dimension w.r.t. that dimension's level-to-grid-point-count function.
   *
   * @throws sgpp::base::not_implemented_exception if the nestedness of some node-generation
   * function is unknown for its level-to-grid-point-count function.
   */
  bool areNodesOfAllDimsNested() const;

  /// @}

  /// @name Operators
  /// @{

  /// @brief Returns whether both setups are equal in every member except the node storage.
  bool operator==(const SparseGridSetup& other) const;

  /// @}

 private:
  LvlType boundaryLevelOffset;                ///< See @ref getBoundaryLevelOffset.
  HyperCubeArea domain;                       ///< Per-dimension @c (min, max) intervals.
  std::vector<NodeFamily*> nodeFamilies;      ///< Per-dimension node generators (non-owning).
  std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs;  ///< Per-dimension growth functions.
  std::shared_ptr<NodeStorage> nodeStorage;   ///< Storage of the 1D node lists.
};

}  // namespace combigrid
}  // namespace sgpp
