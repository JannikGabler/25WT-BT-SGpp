#ifndef COMBIGRID_PARETO_MAXIMA_HPP
#define COMBIGRID_PARETO_MAXIMA_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

namespace sgpp {
namespace combigrid {

bool miDominatesMI(const combigrid::MIVec& miVec, size_t miIdx1, size_t miIdx2);

std::vector<size_t> computeParetoMaxima(const MIVec& miVec);

std::vector<size_t> mergeParetoMaxima(const MIVec miVec,
                                      std::vector<std::vector<size_t>> localParetoMaxima);

// std::vector<size_t> computeParetoMaximaSerial(const combigrid::MIVec& miVec, size_t startIdx,
//                                               size_t endIdx);

// template <typename T>
// std::vector<size_t> computeParetoMaximaParallel(const combigrid::MIVec& miVec);

}  // namespace combigrid
}  // namespace sgpp

#endif