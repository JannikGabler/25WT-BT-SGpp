#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class MIVec {
 public:
  MIVec(size_t nDim, size_t nMI);
  MIVec(const std::vector<MI>& mi);

  size_t nDim() const;
  size_t nMI() const;

  const MIType* data() const;

  MIType operator()(size_t miIdx, size_t dim) const;
  MIType& operator()(size_t miIdx, size_t dim);

  MI operator[](size_t miIdx) const;

  void setMI(size_t idx, const MI& mi);

  bool isDownwardsClosed() const;

  MIVec downwardsClosure() const;

  std::vector<size_t> paretoMaxima(bool isDownwardsClosed = false) const;

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<MIType> data_;  // AoS: [idx][dim]
};

}  // namespace combigrid
}  // namespace sgpp

#endif