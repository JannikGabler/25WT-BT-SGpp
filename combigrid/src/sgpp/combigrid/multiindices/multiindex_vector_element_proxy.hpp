#ifndef COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP
#define COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP

#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

class MIVec;  // Forward declaration to avoid circular includes

class MIVecElemProxy {
 public:
  MIVecElemProxy(const MIVec& miVec, MIType& ref);

  MIVecElemProxy& operator=(const MIType value);

  operator MIType() const;

 private:
  const MIVec& miVec;
  MIType& ref;
};

}  // namespace combigrid
}  // namespace sgpp

#endif