// #include <omp.h>
// #include <sgpp/combigrid/constants.hpp>
// #include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
// #include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
// #include <sgpp/combigrid/tools/concurrency.hpp>
// #include <sgpp/combigrid/tools/multiindex_domination.hpp>
// #include <sgpp/combigrid/tools/paretoMaxima.hpp>
// #include <vector>

// namespace sgpp {
// namespace combigrid {
// namespace tools {

// void updateParetoMaximaDWC(const MIVec& miVec, const misc::MIVecLookup& lookup,
//                            std::vector<size_t>& paretoMaxima, const size_t candidateIdx) {
//   MI candidate = miVec[candidateIdx];

//   for (size_t dim = 0; dim < miVec.nDim(); dim++) {
//     if (dim > 0) {
//       candidate[dim - 1]--;
//     }
//     candidate[dim]++;

//     if (lookup.contains(candidate)) {
//       return;
//     }
//   }

//   paretoMaxima.push_back(candidateIdx);
// }

// void updateParetoMaximaNonDWC(const MIVec& miVec, std::vector<size_t>& paretoMaxima,
//                               const size_t candidateIdx) {
//   bool dominated = false;

//   for (auto iter = paretoMaxima.begin(); iter != paretoMaxima.end(); iter++) {
//     if (miDominatesMI(miVec, *iter, candidateIdx)) {
//       dominated = true;
//       break;
//     }
//     if (miDominatesMI(miVec, candidateIdx, *iter)) {
//       iter = paretoMaxima.erase(iter) - 1;
//     }
//   }

//   if (!dominated) {
//     paretoMaxima.push_back(candidateIdx);
//   }
// }

// std::vector<size_t> mergeParetoMax(const MIVec& miVec,
//                                    std::vector<std::vector<size_t>>& localParetoMaxima) {
//   std::vector<size_t> merged;

//   for (const std::vector<size_t>& local : localParetoMaxima) {
//     for (const size_t candidateIdx : local) {
//       updateParetoMaximaNonDWC(miVec, merged, candidateIdx);
//     }
//   }

//   return merged;
// }

// std::vector<size_t> zipParetoMax(std::vector<std::vector<size_t>>& localParetoMaxima) {
//   size_t totalLength = 0;
//   for (size_t i = 0; i < localParetoMaxima.size(); i++) {
//     totalLength += localParetoMaxima[i].size();
//   }

//   std::vector<size_t> result;
//   result.reserve(totalLength);

//   for (const std::vector<size_t>& local : localParetoMaxima) {
//     result.insert(result.end(), local.begin(), local.end());
//   }

//   return result;
// }

// std::vector<size_t> computeParetoMaxSerialDWC(const combigrid::MIVec& miVec) {
//   std::vector<size_t> paretoMaxima;

//   const misc::MIVecLookup lookup(miVec);

//   for (size_t candidateIdx = 0; candidateIdx < miVec.nMI(); candidateIdx++) {
//     updateParetoMaximaDWC(miVec, lookup, paretoMaxima, candidateIdx);
//   }

//   return paretoMaxima;
// }

// std::vector<size_t> computeParetoMaxSerialNotDWC(const combigrid::MIVec& miVec,
//                                                  const size_t startIdx, const size_t endIdx) {
//   std::vector<size_t> paretoMaxima;

//   for (size_t candidateIdx = startIdx; candidateIdx <= endIdx; candidateIdx++) {
//     updateParetoMaximaNonDWC(miVec, paretoMaxima, candidateIdx);
//   }

//   return paretoMaxima;
// }

// std::vector<size_t> computeParetoMaxParallelDWC(const MIVec& miVec) {
//   const size_t nThreads = omp_get_max_threads();
//   const misc::MIVecLookup lookup(miVec);

//   std::vector<std::vector<size_t>> localParetoMaxima(nThreads);

// #pragma omp parallel
//   {
//     const int threadId = omp_get_thread_num();

// #pragma omp for schedule(static)
//     for (size_t candidateIdx = 0; candidateIdx < miVec.nMI(); candidateIdx++) {
//       updateParetoMaximaDWC(miVec, lookup, localParetoMaxima[threadId], candidateIdx);
//     }
//   }

//   return zipParetoMax(localParetoMaxima);
// }

// std::vector<size_t> computeParetoMaxParallelNonDWC(const MIVec& miVec) {
//   // const size_t length = miVec.nMI() * miVec.nDim();
//   const size_t minBatchSize =
//       (constants::mi_vec::PM_MIN_MIVEC_BATCH_LENGTH_PER_THREAD + miVec.nDim() - 1) /
//       miVec.nDim();

//   const std::vector<size_t> partitioning = tools::partitionRange(miVec.nMI(), minBatchSize);
//   std::vector<std::vector<size_t>> localParetoMaxima(partitioning.size() - 1);

// #pragma omp parallel num_threads(partitioning.size() - 1)
//   {
//     const int threadId = omp_get_thread_num();

//     localParetoMaxima[threadId] =
//         computeParetoMaxSerialNotDWC(miVec, partitioning[threadId], partitioning[threadId + 1] -
//         1);
//   }

//   return mergeParetoMax(miVec, localParetoMaxima);
// }

// std::vector<size_t> computeParetoMaxima(const MIVec& miVec, const bool isDownwardsClosed) {
//   const size_t length = miVec.nMI() * miVec.nDim();
//   if (length == 0) {
//     return std::vector<size_t>{};
//   }

//   if (isDownwardsClosed) {
//     if (length < constants::mi_vec::PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
//       return computeParetoMaxSerialDWC(miVec);
//     } else {
//       return computeParetoMaxParallelDWC(miVec);
//     }
//   } else {
//     if (length < constants::mi_vec::PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY) {
//       return computeParetoMaxSerialNotDWC(miVec, 0, miVec.nMI() - 1);
//     } else {
//       return computeParetoMaxParallelNonDWC(miVec);
//     }
//   }
// }

// }  // namespace tools
// }  // namespace combigrid
// }  // namespace sgpp
