#ifndef COMBIGRID_MULTIINDEX_HPP
#define COMBIGRID_MULTIINDEX_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

template <typename T>
class MI : public std::vector<T> {
 public:
  using std::vector<T>::vector;  // Inherit constructors of std::vector

  size_t toLinearIndex() const;

  size_t productofElems() const;

  size_t nDim() const;

  bool operator<(MI& other) const;
  bool operator>(MI& other) const;

  bool operator<=(MI& other) const;
  bool operator>=(MI& other) const;
};

}  // namespace combigrid
}  // namespace sgpp

#endif

#include <sgpp/combigrid/multiindices/multiindex.tpp>