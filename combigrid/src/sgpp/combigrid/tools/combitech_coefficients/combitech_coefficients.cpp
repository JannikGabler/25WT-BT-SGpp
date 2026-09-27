#include <cstring>
#include <memory>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/bounding_boxes/discrete_unit_bounding_box.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/multiindex_view.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

std::vector<CTCoeffType> computeCTCoeffs(const LvlMIVec& miVec) {
  const std::shared_ptr<LvlMIVecLookup> lookup = miVec.lookup();
  const size_t nMI = miVec.nMI();

  std::vector<int> currentCoeff(nMI, 1);
  std::vector<int> nextCoeff(nMI);

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
    std::memcpy(nextCoeff.data(), currentCoeff.data(), nMI * sizeof(int));

#pragma omp parallel if (nMI >= constants::ct_coefficients::MIN_MIS_FOR_CONCURRENCY)
    {
      // Per-thread scratch multi-index, reused for every successor (no allocation per MI).
      LvlMI successor(miVec.nDim());

#pragma omp for schedule(static)
      for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
        successor = miVec[miIdx];
        successor[dim]++;

        const size_t succIdx = lookup->find(successor);

        if (succIdx < nMI) {
          nextCoeff[miIdx] = currentCoeff[miIdx] - currentCoeff[succIdx];
        } else {
          nextCoeff[miIdx] = currentCoeff[miIdx];
        }
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

#pragma omp parallel
  {
    LvlMI succMI(miVec.nDim());  // Per-thread scratch buffer.

#pragma omp for schedule(static)
    for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
      coeff[miIdx] = ct_coeff_calc::internalComputeCTCoeffSingle(miVec[miIdx], miVec, *lookup,
                                                                 offsets, succMI);
    }
  }

  return coeff;
}

int computeCTCoeffSingle(const LvlMI& mi, const LvlMIVec& miVec) {
  const misc::DiscUnitBB<LvlType> offsets(miVec.nDim());
  const std::shared_ptr<misc::MIVecLookup<LvlType>> lookup = miVec.lookup();

  LvlMI succMI(mi.nDim());

  return ct_coeff_calc::internalComputeCTCoeffSingle(mi, miVec, *lookup, offsets, succMI);
}

/******************
Internal operations
******************/
namespace ct_coeff_calc {

/*
succMI is a scratch buffer, reused across calls to avoid allocations.
 */
CTCoeffType internalComputeCTCoeffSingle(const MIView<LvlType> mi, const LvlMIVec& miVec,
                                         const misc::MIVecLookup<LvlType>& lookup,
                                         const misc::DiscUnitBB<LvlType>& offsets, LvlMI& succMI) {
  assert(mi.nDim() == miVec.nDim() && offsets.nDim == miVec.nDim());

  const size_t nMI = miVec.nMI();
  CTCoeffType coeff = 0;

  for (const std::vector<LvlType>& offset : offsets) {
    succMI = mi;
    succMI += offset;  // Throws std::logic_error on a size mismatch, like mi + offset.
    const size_t succIdx = lookup.find(succMI);

    if (succIdx < nMI) {
      coeff += computeParityOfMI(offset);
    }
  }

  return coeff;
}

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

}  // namespace ct_coeff_calc

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp