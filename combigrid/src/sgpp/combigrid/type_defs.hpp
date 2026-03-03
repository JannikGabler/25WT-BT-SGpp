#pragma once

#include <cstdint>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/hash/std_pair_hash.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/node_generation_functions/node_generation_function.hpp>
#include <unordered_map>

namespace sgpp {
namespace combigrid {

/******************************************************************************************
-Variable types-
Can be changed to better suit specific needs.
Larger data types are necessary when dealing with higher levels, coefficients, or number of
gridpoints. Smaller data types may increase performance.
******************************************************************************************/
using LvlType = unsigned int;
using LvlMI = MI<LvlType>;
using LvlMIVec = MIVec<LvlType>;
using LvlMIVecLookup = misc::MIVecLookup<LvlType>;

using GPCntType = unsigned int;
using GPMI = MI<GPCntType>;

using CTCoeffType = int;

/*********************
-Fixed types-
Should not be changed.
*********************/
using SourceFunc = double (*)(sgpp::base::DataVector);

// using NodeGenFunc = sgpp::base::DataVector (*)(GPCntType);
using Lvl2GPCntFunc = GPCntType (*)(LvlType);
// using NodeGenFuncId = uint64_t;

using SGGenNodeLookup = std::unordered_map<std::pair<NodeGenFunc*, GPCntType>, base::DataVector,
                                           misc::PairHash<NodeGenFunc*, GPCntType>>;

}  // namespace combigrid
}  // namespace sgpp