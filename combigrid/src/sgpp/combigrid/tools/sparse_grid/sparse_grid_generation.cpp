#include <memory>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <unordered_map>
#include <utility>
#include <vector>
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"
#include "sgpp/combigrid/multiindices/multiindex_vector.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

std::unordered_map<std::pair<GPGenFunc, GPCntType>, base::DataVector> genSGGPLookup(
    const SGGenInstr& genInstr, const MIVec& miVec, const std::vector<CTCoeffType> coeff) {}

std::unordered_map<GPGenFunc, std::vector<GPCntType>> getRequiredGPCntPerGPType(
    const SGGenInstr& genInstr, const MIVec& miVec) {
  const std::shared_ptr<MI> componentWiseMax = miVec.componentWiseMax();
  const std::vector<GPGenFunc> uniqueGPGenFuncs = genInstr.getUniqueGPGenFuncs();

  std::unordered_map<GPGenFunc, std::vector<GPCntType>> result(uniqueGPGenFuncs.size());

  for (const GPGenFunc gpGenFunc : uniqueGPGenFuncs) {
    std::vector<GPCntType> localGPCnts;

    for (size_t dim = 0; dim <) {
    }

    result.emplace(gpGenFunc, localGPCnts);
  }
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp