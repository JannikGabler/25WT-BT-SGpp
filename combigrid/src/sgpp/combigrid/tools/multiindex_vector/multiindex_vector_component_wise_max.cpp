// #include <omp.h>
// #include <sgpp/combigrid/constants.hpp>
// #include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
// #include <sgpp/combigrid/tools/concurrency.hpp>
// #include <sgpp/combigrid/tools/multiindex_vector/multiindex_vector_component_wise_max.hpp>
// #include <vector>

// namespace sgpp {
// namespace combigrid {
// namespace tools {

// MI computeComponentWiseMax(const MIVec& miVec) {
//   if (miVec.nMI() * miVec.nDim() < constants::mi_vec::CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
//     return computeComponentWiseMaxSerial(miVec);
//   } else {
//     return computeComponentWiseMaxParallel(miVec);
//   }
// }

// /*
// TODO: Optimize (used pareto Maxima if cached by the miVec)
//  */
// MI computeComponentWiseMaxSerial(const MIVec& miVec) {
//   MI max(miVec.nDim());

//   for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
//     for (size_t dim = 0; dim < miVec.nDim(); dim++) {
//       max[dim] = std::max(max[dim], miVec(miIdx, dim));
//     }
//   }

//   return max;
// }

// /*
// TODO: Optimize (used pareto Maxima if cached by the miVec)
//  */
// MI computeComponentWiseMaxParallel(const MIVec& miVec) {
//   const std::vector<size_t> partitioning =
//       tools::partitionRange(miVec.nMI(), 1, omp_get_max_threads());

//   std::vector<MI> localMax(partitioning.size() - 1, MI(miVec.nDim()));

// #pragma omp parallel
//   {
//     const size_t threadId = omp_get_thread_num();
//     const size_t startIdx = partitioning[threadId];
//     const size_t endIdx = partitioning[threadId + 1] - 1;

//     for (size_t miIdx = startIdx; miIdx <= endIdx; miIdx++) {
//       for (size_t dim = 0; dim < miVec.nDim(); dim++) {
//         localMax[threadId][dim] = std::max(localMax[threadId][dim], miVec(miIdx, dim));
//       }
//     }
//   }

//   return mergeComponentWiseMax(miVec.nDim(), localMax);
// }

// MI mergeComponentWiseMax(const size_t nDim, const std::vector<MI>& localMax) {
//   MI globalMax(nDim);

//   for (const MI& localMI : localMax) {
//     for (size_t dim = 0; dim < nDim; dim++) {
//       globalMax[dim] = std::max(globalMax[dim], localMI[dim]);
//     }
//   }

//   return globalMax;
// }

// }  // namespace tools
// }  // namespace combigrid
// }  // namespace sgpp