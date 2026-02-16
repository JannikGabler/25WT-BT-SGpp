#include <cmath>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>
#include "sgpp/base/exception/not_implemented_exception.hpp"
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"

namespace sgpp {

namespace combigrid {

namespace {

/*
Used in 'computeCTCoeffsNaive'.
Testing purposes only (very inefficient).
 */
void incrementOffsetMI(MI& offsetMI) {
  for (size_t dim = 0; dim < offsetMI.nDim(); dim++) {
    if (offsetMI[dim] == 1) {
      offsetMI[dim] = 0;
    } else {
      offsetMI[dim] = 1;
      return;
    }
  }
}

/*
Used in 'computeCTCoeffsNaive'.
Testing purposes only (very inefficient).
 */
size_t findMIinMIVec(const MI& mi, const MIVec& miVec) {
  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    if (miVec[miIdx] == mi) {
      return miIdx;
    }
  }

  return miVec.nMI();
}

/*
Used in 'computeCTCoeffsNaive'.
 */
size_t fastExp(size_t base, size_t exp) {
  size_t result = 1;

  while (exp > 0) {
    if (exp & 1) result *= base;
    base *= base;
    exp >>= 1;
  }

  return result;
}

/*
Used in 'computeCTCoeffsNaive'.
Testing purposes only (very inefficient).
 */
int computeParityOfMI(const MI& mi) {
  int parity = 1;

  for (const MIType v : mi) {
    if (v != 0) {
      parity *= -1;
    }
  }

  return parity;
}

}  // namespace

namespace tools {

std::vector<int> computeCTCoeffs(const MIVec& miVec) {
  const misc::MIVecLookup lookup(miVec);
  const size_t nMI = miVec.nMI();

  std::vector<int> currentCoeff(nMI, 1);
  std::vector<int> nextCoeff(nMI, 0);

  for (size_t dim = 0; dim < miVec.nDim(); dim++) {
#pragma omp parallel for schedule(static) if (nMI >= ct_coefficients::MIN_MIS_FOR_CONCURRENCY)
    for (size_t miIdx = 0; miIdx < nMI; miIdx++) {
      MI successor = miVec[miIdx];
      successor[dim]++;

      const size_t succIdx = lookup.find(successor);

      if (succIdx < nMI) {
        nextCoeff[miIdx] = currentCoeff[miIdx] - currentCoeff[succIdx];
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

std::vector<int> computeCTCoeffsNaive(const MIVec& miVec) {
  const size_t nSuccessors = fastExp(2, miVec.nDim()) - 1;

  std::vector<int> coeff(miVec.nMI(), 1);

  for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
    const MI mi = miVec[miIdx];

    MI offsetMI(miVec.nDim(), 0);
    for (size_t i = 0; i < nSuccessors; i++) {
      incrementOffsetMI(offsetMI);

      const MI succMI = mi + offsetMI;
      const size_t succIdx = findMIinMIVec(succMI, miVec);

      coeff[succIdx] += computeParityOfMI(offsetMI);
    }
  }
  return coeff;
}

}  // namespace tools

}  // namespace combigrid

}  // namespace sgpp