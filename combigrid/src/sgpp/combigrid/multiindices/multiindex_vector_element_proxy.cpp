#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_element_proxy.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

MIVecElemProxy::MIVecElemProxy(const MIVec& miVec, MIType& ref) : miVec(miVec), ref(ref) {}

MIVecElemProxy& MIVecElemProxy::operator=(const MIType value) {
  miVec.clearCachedValues();
  ref = value;
  return *this;
}

MIVecElemProxy::operator MIType() const { return ref; }

}  // namespace combigrid
}  // namespace sgpp