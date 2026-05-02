#ifndef COMBIGRID_SGGENINSTRUCTION_HPP
#define COMBIGRID_SGGENINSTRUCTION_HPP

#include <memory>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {

class SGGenInstr {
 public:
  /***********
  Constructors
  ***********/
  SGGenInstr(size_t nDim);

  /**********
  Destructors
  **********/
  virtual ~SGGenInstr() = default;

  /******
  Getters
  ******/
  size_t nDim() const;

  LvlType getBoundaryLevelOffset() const;

  const HyperCubeArea& getBounds() const;
  std::pair<double, double> getBoundsForDim(size_t dim) const;

  const std::vector<NodeGenFunc*>& getNodeGenFuncs() const;
  NodeGenFunc* getNodeGenFuncForDim(size_t dim) const;

  const std::vector<Lvl2GPCntFunc>& getLvl2GPCntFuncs() const;
  Lvl2GPCntFunc getLvl2GPCntFuncForDim(size_t dim) const;

  /******
  Setters
  ******/
  void setBoundaryLevelOffset(LvlType boundaryLevelOffset);

  void setBoundsForDim(std::pair<double, double> interval, size_t dim);
  void setBounds(std::pair<double, double> interval);
  void setBounds(const HyperCubeArea& bounds);

  void setNodeGenFuncForDim(NodeGenFunc* nodeGenFunc, size_t dim);
  void setNodeGenFunc(NodeGenFunc* nodeGenFunc);
  void setNodeGenFuncs(const std::vector<NodeGenFunc*>& nodeGenFuncs);

  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);
  void setLvl2GPCntFunc(Lvl2GPCntFunc lvl2GPCntFunc);
  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs);

  /*********************
  Sparse grid generation
  *********************/
  virtual LvlMIVec genMIVec() const = 0;

  virtual std::pair<LvlMIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const = 0;

  /****************
  Helper operations
  ****************/
  virtual std::shared_ptr<SGGenInstr> clone() const = 0;

  void resize(size_t newDimCnt);

  double getVolumeOfBounds() const;

  std::vector<NodeGenFunc*> getUniqueNodeGenFuncs() const;

 private:
  LvlType boundaryLevelOffset;

  HyperCubeArea bounds;
  std::vector<NodeGenFunc*> nodeGenFuncs;
  std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs;
};

}  // namespace combigrid
}  // namespace sgpp

#endif /* SGGENINSTRUCTION_HPP */
