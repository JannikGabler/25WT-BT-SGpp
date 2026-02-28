#ifndef COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP
#define COMBIGRID_TOOLS_MULTIINDEX_VECTOR_COMPONENT_WISE_MAX_HPP

#include <omp.h>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/tools/concurrency.hpp>
#include <vector>

template <typename T>
class MIVec;  // Forward Declaration

namespace sgpp {
namespace combigrid {
namespace tools {

template <typename T>
MI<T> mergeComponentWiseMax(const size_t nDim, const std::vector<MI<T>>& localMax);

/*
TODO: Optimize (used pareto Maxima if cached by the miVec)
 */
template <typename T>
MI<T> computeComponentWiseMaxParallel(const MIVec<T>& miVec);

/*
TODO: Optimize (used pareto Maxima if cached by the miVec)
 */
template <typename T>
MI<T> computeComponentWiseMaxSerial(const MIVec<T>& miVec);

template <typename T>
MI<T> computeComponentWiseMax(const MIVec<T>& miVec);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.tpp>

#endif