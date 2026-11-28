/**
 * @file sparse_grid_generation_instruction.hpp
 * @brief Abstract base class for sparse-grid generation instructions.
 *
 * A "generation instruction" carries everything needed to materialize a
 * combination-technique sparse grid: the spatial domain, the per-dimension
 * node families, the per-dimension growth functions, and the
 * algorithmic recipe that produces the level multi-indices and their
 * combination coefficients.
 */
#ifndef COMBIGRID_SGGENINSTRUCTION_HPP
#define COMBIGRID_SGGENINSTRUCTION_HPP

#include <memory>
#include <sgpp/combigrid/node_families/node_family.hpp>
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
 * matching combination coefficients of a specific combination scheme.
 */
class SGGenInstr {
 public:
  /***********
  Constructors
  ***********/
  /**
   * @brief Constructs a generator for an @p nDim -dimensional sparse grid.
   *
   * Default state: domain @f$[0,1]^{\mathrm{nDim}}@f$, no node families,
   * no growth functions, boundary level offset @c 0.
   *
   * @param nDim Spatial dimensionality.
   */
  SGGenInstr(size_t nDim);

  /**********
  Destructors
  **********/
  virtual ~SGGenInstr() = default;

  /******
  Getters
  ******/
  /// @brief Returns the spatial dimensionality.
  size_t nDim() const;

  /**
   * @brief Returns the boundary level offset.
   *
   * Tensor grids whose level in some dimension is @c >= the offset use
   * boundary nodes in that dimension. A value of @c 0 means every level
   * uses the boundary; a very large value disables boundary use altogether.
   */
  LvlType getBoundaryLevelOffset() const;

  /// @brief Returns the per-dimension domain (one @c (min, max) interval per dim).
  const HyperCubeArea& getDomain() const;

  /// @brief Returns the @c (min, max) interval for dimension @p dim.
  std::pair<double, double> getDomainForDim(size_t dim) const;

  /// @brief Returns the per-dimension node families.
  const std::vector<NodeFamily*>& getNodeFamilies() const;

  /// @brief Returns the node family for dimension @p dim.
  NodeFamily* getNodeFamilyForDim(size_t dim) const;

  /// @brief Returns the per-dimension level-to-grid-point-count growth functions.
  const std::vector<Lvl2GPCntFunc>& getLvl2GPCntFuncs() const;

  /// @brief Returns the level-to-count growth function for dimension @p dim.
  Lvl2GPCntFunc getLvl2GPCntFuncForDim(size_t dim) const;

  /******
  Setters
  ******/
  /// @brief Sets the boundary level offset (see @ref getBoundaryLevelOffset).
  void setBoundaryLevelOffset(LvlType boundaryLevelOffset);

  /// @brief Replaces the @p dim -th component of the domain.
  void setDomainForDim(std::pair<double, double> interval, size_t dim);
  /// @brief Sets every dimension to the same @p interval.
  void setDomain(std::pair<double, double> interval);
  /// @brief Replaces the entire domain.
  void setDomain(const HyperCubeArea& domain);

  /// @brief Sets the node family for dimension @p dim.
  void setNodeFamilyForDim(NodeFamily* nodeFamily, size_t dim);
  /// @brief Uses the same node family for every dimension.
  void setNodeFamily(NodeFamily* nodeFamily);
  /// @brief Replaces the per-dimension node families.
  void setNodeFamilies(const std::vector<NodeFamily*>& nodeFamilies);

  /// @brief Sets the level-to-count growth function for dimension @p dim.
  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);
  /// @brief Uses the same growth function for every dimension.
  void setLvl2GPCntFunc(Lvl2GPCntFunc lvl2GPCntFunc);
  /// @brief Replaces the per-dimension growth functions.
  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs);

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

  /****************
  Helper operations
  ****************/
  /**
   * TODO: Document
   */
  virtual std::unique_ptr<SGGenInstr> clone() const = 0;

  /**
   * @brief Resizes the instruction to a different dimensionality.
   *
   * Per-dimension settings (domain / node generators / growth functions)
   * are extended by repeating the last entry, or truncated to the new
   * length.
   *
   * @param newDimCnt New dimensionality.
   */
  void resize(size_t newDimCnt);

  /// @brief Returns the volume of the domain.
  double getVolumeOfDomain() const;

  /// @brief Returns the unique node families used across all dimensions.
  std::vector<NodeFamily*> getUniqueNodeFamilies() const;

  /**
   * @brief Returns whether the nodes are nested (see @c NodeFamily::isNested) in every
   * dimension w.r.t. that dimension's level-to-grid-point-count function.
   *
   * @throws sgpp::base::not_implemented_exception if the nestedness of some node family
   * is unknown for its level-to-grid-point-count function.
   */
  bool areNodesOfAllDimsNested() const;

 private:
  LvlType boundaryLevelOffset;  ///< See @ref getBoundaryLevelOffset.

  HyperCubeArea domain;                       ///< Per-dimension @c (min, max) intervals.
  std::vector<NodeFamily*> nodeFamilies;      ///< Per-dimension node generators (non-owning).
  std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs;  ///< Per-dimension growth functions.
};

}  // namespace combigrid
}  // namespace sgpp

#endif /* SGGENINSTRUCTION_HPP */
