/**
 * @file sg_gen_instruction.hpp
 * @brief Abstract base class for sparse-grid generation instructions.
 *
 * A "generation instruction" carries everything needed to materialize a
 * combination-technique sparse grid: the spatial domain, the per-dimension
 * node-generation rules, the per-dimension growth functions, and the
 * algorithmic recipe that produces the level multi-indices and their
 * combination coefficients.
 */
#ifndef COMBIGRID_SGGENINSTRUCTION_HPP
#define COMBIGRID_SGGENINSTRUCTION_HPP

#include <memory>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
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
 * @c MIVecSGGenInstr) implement @ref genMIVec and
 * @ref genMIVecWithCoeff to produce the level multi-indices and the
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
   * Default state: domain @f$[0,1]^{\mathrm{nDim}}@f$, no node-generation
   * functions, no growth functions, boundary level offset @c 0.
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

  /// @brief Returns the per-dimension node-generation functions.
  const std::vector<NodeGenFunc*>& getNodeGenFuncs() const;

  /// @brief Returns the node-generation function for dimension @p dim.
  NodeGenFunc* getNodeGenFuncForDim(size_t dim) const;

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

  /// @brief Sets the node-generation function for dimension @p dim.
  void setNodeGenFuncForDim(NodeGenFunc* nodeGenFunc, size_t dim);
  /// @brief Uses the same node-generation function for every dimension.
  void setNodeGenFunc(NodeGenFunc* nodeGenFunc);
  /// @brief Replaces the per-dimension node-generation functions.
  void setNodeGenFuncs(const std::vector<NodeGenFunc*>& nodeGenFuncs);

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
   * @brief Generates the level multi-indices participating in the
   * combination.
   * @return Level-multi-index vector.
   */
  virtual LvlMIVec genMIVec() const = 0;

  /**
   * @brief Generates the level multi-indices and their combination
   * coefficients in one pass.
   * @return Pair @c (multi-indices, coefficients) aligned by index.
   */
  virtual std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const = 0;

  /****************
  Helper operations
  ****************/
  /**
   * @brief Polymorphic copy.
   * @return Independent shared-pointer copy of this instruction.
   */
  virtual std::shared_ptr<SGGenInstr> clone() const = 0;

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

  /// @brief Returns the unique node-generation functions used across all dimensions.
  std::vector<NodeGenFunc*> getUniqueNodeGenFuncs() const;

 private:
  LvlType boundaryLevelOffset;  ///< See @ref getBoundaryLevelOffset.

  HyperCubeArea domain;                          ///< Per-dimension @c (min, max) intervals.
  std::vector<NodeGenFunc*> nodeGenFuncs;        ///< Per-dimension node generators (non-owning).
  std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs;     ///< Per-dimension growth functions.
};

}  // namespace combigrid
}  // namespace sgpp

#endif /* SGGENINSTRUCTION_HPP */
