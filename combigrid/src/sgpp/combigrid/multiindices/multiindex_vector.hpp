#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>
#include "sgpp/combigrid/multiindices/multiindex.hpp"

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec {
 public:
  MIVec(size_t nDim, size_t nMI);
  MIVec(const std::vector<MI<T>>& mi);

  size_t nDim() const;
  size_t nMI() const;

  T operator()(size_t miIdx, size_t dim) const;
  T& operator()(size_t miIdx, size_t dim);

  MI<T> operator[](size_t miIdx) const;

  void setMI(size_t idx, MI<T> mi);

  bool isDownwardsClosed() const;

  std::vector<size_t> paretoMaximum(bool assumeDownwardsClosed = false) const;

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<T> data;  // AoS: [idx][dim]
};

}  // namespace combigrid
}  // namespace sgpp

#include <sgpp/combigrid/multiindices/multiindex_vector.tpp>

#endif