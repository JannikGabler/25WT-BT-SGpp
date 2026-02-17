#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <memory>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
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

  const std::shared_ptr<MI> componentWiseMax() const;

  const std::shared_ptr<std::vector<size_t>> paretoMaxima(bool isDownwardsClosed = false) const;

  const std::shared_ptr<misc::MIVecLookup> lookup() const;

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<MIType> data_;  // AoS: [idx][dim]

  mutable std::shared_ptr<MI> componentWiseMax_;
  mutable std::shared_ptr<std::vector<size_t>> paretoMaxima_;
  mutable std::shared_ptr<misc::MIVecLookup> lookup_;

  void clearCachedValues() const;
};

}  // namespace combigrid
}  // namespace sgpp

#endif