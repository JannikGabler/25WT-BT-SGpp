#ifndef COMBIGRID_MULTIINDEX_DOMINATION_HPP
#define COMBIGRID_MULTIINDEX_DOMINATION_HPP

#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <vector>
#include "sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

bool miDominatesMI(const MIVec& miVec, size_t miIdx1, size_t miIdx2);
bool miDominatesMI(const MIVec& miVec, size_t miVecIdx, const MI& mi);
bool miDominatesMI(const MIVec& miVec, size_t miVecIdx, const std::vector<MIType>& mi);

bool miVecDominatesMI(const MIVec& miVec, const MI& mi);
bool miVecDominatesMI(const MIVec& miVec, const std::vector<MIType>& mi);
bool miVecDominatesMI(const MIVec& miVec, const std::vector<size_t>& miVecIdx, const MI& mi);
bool miVecDominatesMI(const MIVec& miVec, const std::vector<size_t>& miVecIdx,
                      const std::vector<MIType>& mi);

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp

#endif