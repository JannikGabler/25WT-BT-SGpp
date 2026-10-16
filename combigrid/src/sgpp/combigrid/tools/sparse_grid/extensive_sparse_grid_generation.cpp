#include <algorithm>
#include <cassert>
#include <iterator>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/sparse_grid/extensive_sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation.hpp>
#include <sgpp/combigrid/tools/sparse_grid/sparse_grid_generation_node_lookup.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <utility>
#include <vector>

using namespace sgpp::combigrid::tools::sparse_grid_generation;
using namespace sgpp::combigrid::tools::extensive_sparse_grid_generation;

namespace sgpp {
namespace combigrid {

namespace tools {

void populateExtensiveSG(ExtensiveSparseGrid& sg, const SGGenInstr& genInstr, const LvlMIVec& miVec,
                         const std::vector<CTCoeffType>& coeffs) {
  assert(sg.nDim() == genInstr.nDim() && sg.nDim() == miVec.nDim());
  assert(miVec.nMI() == coeffs.size());
  assert(areTGsCorrectlyPartitionedForExtensiveSG(coeffs));

  const size_t nTG = miVec.nMI();

  const auto partitionPoint = std::partition_point(
      coeffs.begin(), coeffs.end(), [](const CTCoeffType& coeff) { return coeff == 0; });
  const size_t nTGWithCoeffZero =
      static_cast<size_t>(std::distance(coeffs.begin(), partitionPoint));

  std::vector<TensorGridCTData> zeroCoeffTGData =
      genTGData(genInstr, miVec.getView().first(nTGWithCoeffZero),
                std::span<const CTCoeffType>(coeffs).first(nTGWithCoeffZero));
  std::vector<TensorGridCTData> nonZeroCoeffTGData =
      genTGData(genInstr, miVec.getView().last(nTG - nTGWithCoeffZero),
                std::span<const CTCoeffType>(coeffs).last(nTG - nTGWithCoeffZero));

  sg.setTensorGrids(std::move(nonZeroCoeffTGData));
  sg.setTensorGridsWithCoeffZero(std::move(zeroCoeffTGData));
}

/// @brief Internal implementation details of the extensive-sparse-grid generation.
namespace extensive_sparse_grid_generation {

size_t partitionForExtensiveSG(LvlMIVec& miVec, std::vector<CTCoeffType>& coeffs) {
  assert(miVec.nMI() == coeffs.size());

  size_t writeIdx = 0;
  LvlMIVec tmpMIVec(miVec.nDim(), 0);
  std::vector<CTCoeffType> tmpCoeffs;

  for (size_t i = 0; i < miVec.nMI(); i++) {
    if (coeffs[i] == 0) {
      miVec.moveMI(writeIdx, i);  // case writeIdx == i is already covered in MIVec::moveMI
      writeIdx++;
    } else {
      tmpMIVec.push_back(miVec[i]);
      tmpCoeffs.push_back(coeffs[i]);
    }
  }

  std::fill(coeffs.begin(), coeffs.begin() + static_cast<std::ptrdiff_t>(writeIdx), 0);
  std::move(tmpMIVec.begin(), tmpMIVec.end(),
            miVec.begin() + static_cast<std::ptrdiff_t>(writeIdx));
  std::copy(tmpCoeffs.begin(), tmpCoeffs.end(),
            coeffs.begin() + static_cast<std::ptrdiff_t>(writeIdx));

  return writeIdx;
}

bool areTGsCorrectlyPartitionedForExtensiveSG(const std::vector<CTCoeffType>& coeffs) {
  bool hasSeenNonZeroCoeff = false;

  for (size_t i = 0; i < coeffs.size(); i++) {
    if (coeffs[i] != 0) {
      hasSeenNonZeroCoeff = true;
    } else if (hasSeenNonZeroCoeff) {
      return false;
    }
  }

  return true;
}

}  // namespace extensive_sparse_grid_generation

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp