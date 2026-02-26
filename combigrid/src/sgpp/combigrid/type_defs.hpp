#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/hash/std_pair_hash.hpp>
#include <unordered_map>

namespace sgpp {
namespace combigrid {

/******************************************************************************************
-Variable types-
Can be changed to better suit specific needs.
Larger data types are necessary when dealing with higher levels, coefficients, or number of
gridpoints. Smaller data types may increase performance.
******************************************************************************************/
using MIType = unsigned int;
using CTCoeffType = int;
using GPCntType = size_t;

/*********************
-Fixed types-
Should not be changed.
*********************/
using NodeGenFunc = sgpp::base::DataVector (*)(GPCntType);
using Lvl2GPCntFunc = GPCntType (*)(MIType);

using SGGenNodeLookup = std::unordered_map<std::pair<NodeGenFunc, GPCntType>, base::DataVector,
                                           misc::PairHash<NodeGenFunc, GPCntType>>;

}  // namespace combigrid
}  // namespace sgpp