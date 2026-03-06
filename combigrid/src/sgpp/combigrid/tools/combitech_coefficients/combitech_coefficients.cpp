#include <cstring>
#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace {

/*
Returns 1 if the number of elements in mi \neq 0 is even and -1 else.
 */
CTCoeffType computeParityOfMI(const std::vector<LvlType>& mi) {
  size_t cnt = 0;

  for (const LvlType v : mi) {
    cnt += (v != 0);
  }

  if (cnt & 1) {  // Odd
    return -1;
  } else {  // Even
    return 1;
  }
}

CTCoeffType computeCTCoeffSingleInternal(const LvlMI& mi, const LvlMIVec& miVec,
                                         const misc::MIVecLookup<LvlType>& lookup,
                                         const misc::DiscUnitBB<LvlType>& offsets) {
  assert(mi.nDim() == miVec.nDim() && offsets.nDim == miVec.nDim());

  const size_t nMI = miVec.nMI();
  CTCoeffType coeff = 0;

  for (const std::vector<LvlType>& offset : offsets) {
    const LvlMI succMI = mi + offset;
    const size_t succIdx = lookup.find(succMI);

    if (succIdx < nMI) {
      coeff += computeParityOfMI(offset);
    }
  }

  return coeff;
}

}  // namespace

namespace tools {

std::vector<CTCoeffType> computeCTCoeffs(const LvlMIVec& miVec) {
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

std::vector<CTCoeffType> computeCTCoeffsNaive(const LvlMIVec& miVec) {
  const misc::DiscUnitBB<LvlType> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup<LvlType>> lookup = miVec.lookup();
  std::vector<CTCoeffType> coeff(miVec.nMI(), 0);

#pragma omp parallel for schedule(static)
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    coeff[miIdx] = computeCTCoeffSingleInternal(miVec[miIdx], miVec, *lookup, offsets);
  }

  return coeff;
}

int computeCTCoeffSingle(const LvlMI& mi, const LvlMIVec& miVec) {
  const misc::DiscUnitBB<LvlType> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup<LvlType>> lookup = miVec.lookup();

  return computeCTCoeffSingleInternal(mi, miVec, *lookup, offsets);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp