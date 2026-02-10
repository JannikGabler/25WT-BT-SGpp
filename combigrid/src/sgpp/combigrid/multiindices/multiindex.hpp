#ifndef COMBIGRID_MULTIINDEX_HPP
#define COMBIGRID_MULTIINDEX_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class MI : public std::vector<MIType> {
 public:
  using std::vector<MIType>::vector;  // Inherit constructors of std::vector

  size_t toLinearIndex() const;

  size_t productofElems() const;

  size_t nDim() const;

  bool operator==(const MI& other) const;

  bool operator<(const MI& other) const;
  bool operator>(const MI& other) const;

  bool operator<=(const MI& other) const;
  bool operator>=(const MI& other) const;
};

}  // namespace combigrid
}  // namespace sgpp

#endif