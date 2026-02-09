#include <sgpp/globaldef.hpp>
#include "sgpp/base/datatypes/DataVector.hpp"

namespace sgpp {
namespace combigrid {

using GPGenFunc = sgpp::base::DataVector (*)(unsigned int);
using Lvl2GPCntFunc = std::vector<unsigned int> (*)(unsigned int);

}  // namespace combigrid
}  // namespace sgpp