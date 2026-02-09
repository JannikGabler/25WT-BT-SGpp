#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

namespace sgpp {
namespace combigrid {

template <typename T>
bool miDominatesMI(const combigrid::MIVec<T>& miVec, size_t miIdx1, size_t miIdx2);

template <typename T>
std::vector<size_t> computeParetoMaximaSerial(const combigrid::MIVec<T>& miVec, size_t startIdx,
                                              size_t endIdx);

}  // namespace combigrid
}  // namespace sgpp

#include <sgpp/combigrid/multiindices/paretoMaxima.tpp>