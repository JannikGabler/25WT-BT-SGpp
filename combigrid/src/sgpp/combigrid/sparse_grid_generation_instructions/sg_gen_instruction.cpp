#include <algorithm>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>
#include <vector>

namespace sgpp {

namespace combigrid {

// TODO: Set default of lvl2GPCntFuncs
SGGenInstr::SGGenInstr(const size_t dimCount)
    : bounds(dimCount, {0, 1}), gPGenFuncs(dimCount, nullptr), lvl2GPCntFuncs(dimCount, nullptr) {}

size_t sgpp::combigrid::SGGenInstr::nDim() const { return this->bounds.size(); }

void SGGenInstr::setBoundForDim(const std::pair<double, double> bound, const size_t dim) {
  bounds[dim] = bound;
}

void SGGenInstr::setBounds(const std::vector<std::pair<double, double>>& bounds) {
  const size_t nElemsToCopy = std::min(this->bounds.size(), bounds.size());
  std::copy_n(bounds.begin(), nElemsToCopy, this->bounds.begin());
}

void SGGenInstr::setGPGenFuncForDim(const GPGenFunc gPGenFunc, const size_t dim) {
  this->gPGenFuncs.at(dim) = gPGenFunc;
}

void SGGenInstr::setGPGenFuncs(const std::vector<GPGenFunc>& gPGenFuncs) {
  const size_t nElemsToCopy = std::min(this->gPGenFuncs.size(), gPGenFuncs.size());
  std::copy_n(gPGenFuncs.begin(), nElemsToCopy, this->gPGenFuncs.begin());
}

void SGGenInstr::setLvl2GPCntFuncForDim(const Lvl2GPCntFunc lvl2GPCntFunc, const size_t dim) {
  this->lvl2GPCntFuncs.at(dim) = lvl2GPCntFunc;
}

void SGGenInstr::setLvl2GPCntFuncs(const std::vector<Lvl2GPCntFunc>& lvl2GPCntFuncs, size_t dim) {
  const size_t nElemsToCopy = std::min(this->lvl2GPCntFuncs.size(), lvl2GPCntFuncs.size());
  std::copy_n(lvl2GPCntFuncs.begin(), nElemsToCopy, this->lvl2GPCntFuncs.begin());
}

void SGGenInstr::resize(const size_t newDimCnt) {
  bounds.resize(newDimCnt);
  gPGenFuncs.resize(newDimCnt);
  lvl2GPCntFuncs.resize(newDimCnt);
}

}  // namespace combigrid
}  // namespace sgpp
