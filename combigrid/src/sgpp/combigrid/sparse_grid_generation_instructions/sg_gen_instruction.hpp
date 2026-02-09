#ifndef COMBIGRID_SGGENINSTRUCTION_HPP
#define COMBIGRID_SGGENINSTRUCTION_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <utility>
#include <vector>
#include "sgpp/combigrid/multiindices/multiindex_vector.hpp"

namespace sgpp {
namespace combigrid {

class SGGenInstr {
 public:
  SGGenInstr(size_t dimCount);

  size_t nDim() const;

  void setBoundForDim(std::pair<double, double> bound, size_t dim);

  void setBounds(const std::vector<std::pair<double, double>>& bounds);

  void setGPGenFuncForDim(GPGenFunc gPGenFunc, size_t dim);

  void setGPGenFuncs(const std::vector<GPGenFunc>& gPGenFuncs);

  void setLvl2GPCntFuncForDim(Lvl2GPCntFunc lvl2GPCntFunc, size_t dim);

  void setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs, size_t dim);

  virtual MIVec genParetoMaximum() = 0;

  void resize(size_t newDimCnt);

 private:
  std::vector<std::pair<double, double>> bounds;
  std::vector<GPGenFunc> gPGenFuncs;
  std::vector<std::vector<unsigned int> (*)(unsigned int)> lvl2GPCntFuncs;
};

}  // namespace combigrid
}  // namespace sgpp

#endif /* SGGENINSTRUCTION_HPP */
