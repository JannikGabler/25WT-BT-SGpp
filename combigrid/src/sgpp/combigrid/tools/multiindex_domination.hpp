#ifndef COMBIGRID_MULTIINDEX_DOMINATION_HPP
#define COMBIGRID_MULTIINDEX_DOMINATION_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

bool miDominatesMI(const MIVec& miVec, size_t miIdx1, size_t miIdx2);
bool miDominatesMI(const MIVec& miVec, size_t miVecIdx, const MI& mi);

bool miVecDominatesMI(const MIVec& miVec, const MI& mi);
bool miVecDominatesMI(const MIVec& miVec, const std::vector<size_t> miVecIdx, const MI& mi);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif