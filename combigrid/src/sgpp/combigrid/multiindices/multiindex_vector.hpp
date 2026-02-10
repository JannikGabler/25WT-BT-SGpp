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
  struct MILookupView {
    const MIType* data;
    const size_t n;
  };

  MIVec(size_t nDim, size_t nMI);
  MIVec(const std::vector<MI>& mi);

  size_t nDim() const;
  size_t nMI() const;

  MIType operator()(size_t miIdx, size_t dim) const;
  MIType& operator()(size_t miIdx, size_t dim);

  MI operator[](size_t miIdx) const;

  MILookupView getLookupView(size_t miIdx) const;

  void setMI(size_t idx, MI mi);

  bool isDownwardsClosed() const;

  std::vector<size_t> paretoMaximum(bool assumeDownwardsClosed = true) const;

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<MIType> data;  // AoS: [idx][dim]
};

}  // namespace combigrid
}  // namespace sgpp

#endif