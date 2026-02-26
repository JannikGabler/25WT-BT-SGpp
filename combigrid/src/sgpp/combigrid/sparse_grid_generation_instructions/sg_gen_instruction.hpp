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
  SGGenInstr(size_t nDim);

  size_t nDim() const;

  void setBoundaryIndexOffset(MIType boundaryIndexOffset);

  MIType getBoundaryIndexOffset() const;

  void setBoundForDim(std::pair<double, double> bound, size_t dim);

  void setBounds(const std::vector<std::pair<double, double>>& bounds);

  void setNodeGenFuncForDim(NodeGenFunc gPGenFunc, size_t dim);
  void setNodeGenFuncs(const std::vector<NodeGenFunc>& gPGenFuncs);
  const std::vector<NodeGenFunc>& getNodeGenFuncs() const;
  NodeGenFunc getNodeGenFuncForDim(size_t nDim) const;

  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);
  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs, size_t dim);
  const std::vector<Lvl2GPCntFunc>& getLvl2GPCntFuncs() const;
  Lvl2GPCntFunc getLvl2GPCntFuncForDim(size_t nDim) const;

  virtual MIVec genCompleteMIVec() const = 0;

  virtual std::pair<MIVec, std::vector<CTCoeffType>> genCompleteMIVecWithCoeff() const = 0;

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
