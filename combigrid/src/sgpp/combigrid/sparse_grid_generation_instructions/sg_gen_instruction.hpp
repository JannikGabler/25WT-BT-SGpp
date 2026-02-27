#ifndef COMBIGRID_SGGENINSTRUCTION_HPP
#define COMBIGRID_SGGENINSTRUCTION_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
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

  /******
  Getters
  ******/
  size_t nDim() const;

  MIType getBoundaryIndexOffset() const;

  const std::vector<NodeGenFunc>& getNodeGenFuncs() const;
  NodeGenFunc getNodeGenFuncForDim(size_t dim) const;

  const std::vector<Lvl2GPCntFunc>& getLvl2GPCntFuncs() const;
  Lvl2GPCntFunc getLvl2GPCntFuncForDim(size_t dim) const;

  /******
  Setters
  ******/
  void setBoundaryIndexOffset(MIType boundaryIndexOffset);

  void setBoundForDim(std::pair<double, double> bound, size_t dim);
  void setBounds(const std::vector<std::pair<double, double>>& bounds);

  void setNodeGenFuncForDim(NodeGenFunc gPGenFunc, size_t dim);
  void setNodeGenFuncs(const std::vector<NodeGenFunc>& gPGenFuncs);

  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);
  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs, size_t dim);

  /*********************
  Sparse grid generation
  *********************/
  virtual MIVec genMIVec() const = 0;

  virtual std::pair<MIVec, std::vector<CTCoeffType>> genMIVecWithCoeff() const = 0;

  /****************
  Helper operations
  ****************/
  void resize(size_t newDimCnt);

  std::vector<NodeGenFunc> getUniqueNodeGenFuncs() const;

 private:
  MIType boundaryIndexOffset;

  std::vector<std::pair<double, double>> bounds;
  std::vector<NodeGenFunc> nodeGenFuncs;
  std::vector<Lvl2GPCntFunc> lvl2GPCntFuncs;
};

}  // namespace combigrid
}  // namespace sgpp

#endif /* SGGENINSTRUCTION_HPP */
