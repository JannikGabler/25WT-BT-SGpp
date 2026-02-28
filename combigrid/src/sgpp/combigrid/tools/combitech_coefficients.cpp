#include <cstring>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>
#include "sgpp/combigrid/type_defs.hpp"

namespace sgpp {
namespace combigrid {
namespace {

/*
Used in 'computeCTCoeffsNaive'.
Testing purposes only (very inefficient).
 */
size_t findMIinMIVec(const LvlMI& mi, const LvlMIVec& miVec) {
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    if (miVec[miIdx] == mi) {
      return miIdx;
    }
  }

  return miVec.nMI();
}

/*
Used in 'computeCTCoeffsNaive'.
Testing purposes only (very inefficient).
 */
int computeParityOfMI(const std::vector<LvlType>& mi) {
  int parity = 1;

  for (const LvlType v : mi) {
    if (v != 0) {
      parity *= -1;
    }
  }

  return parity;
}

}  // namespace

namespace tools {

std::vector<int> computeCTCoeffs(const LvlMIVec& miVec) {
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();
  const size_t nMI = miVec.nMI();

  std::vector<int> currentCoeff(nMI, 1);
  std::vector<int> nextCoeff(nMI);

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    std::memcpy(nextCoeff.data(), currentCoeff.data(), nMI * sizeof(int));

#pragma omp parallel for schedule( \
        static) if (nMI >= constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY)
    for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
      LvlMI successor = miVec[miIdx];
      successor[dim]++;

      const size_t succIdx = lookup->find(successor);

      if (succIdx < nMI) {
        nextCoeff[miIdx] = currentCoeff[miIdx] - currentCoeff[succIdx];
      } else {
        nextCoeff[miIdx] = currentCoeff[miIdx];
      }
    }
    currentCoeff.swap(nextCoeff);
  }

  return currentCoeff;
}

std::vector<int> computeClassicCTCoeffs(const size_t maxLvl, const size_t nDim) {
  // TODO
  throw base::not_implemented_exception("Function is not yet implemented!");
}

std::vector<int> computeCTCoeffsNaive(const LvlMIVec& miVec) {
  const misc::DiscUnitBB<LvlType> offsets(miVec.nDim());

  std::vector<int> coeff(miVec.nMI(), 0);

  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const LvlMI mi = miVec[miIdx];

    for (const std::vector<LvlType>& offset : offsets) {
      const LvlMI succMI = mi - offset;
      const size_t succIdx = findMIinMIVec(succMI, miVec);

      if (succIdx < miVec.nMI()) {
        coeff[succIdx] += computeParityOfMI(offset);
      }
    }
  }
  return coeff;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp