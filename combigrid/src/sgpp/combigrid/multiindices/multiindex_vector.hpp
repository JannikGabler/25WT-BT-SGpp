#ifndef COMBIGRID_MULTIINDEX_VECTOR_HPP
#define COMBIGRID_MULTIINDEX_VECTOR_HPP

#include <initializer_list>
#include <memory>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector_element_proxy.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class MIVec {
 public:
  MIVec(size_t nDim, size_t nMI);
  MIVec(const std::vector<MI>& mi);
  MIVec(const std::vector<std::vector<MIType>>& mi);
  MIVec(const std::initializer_list<MI> initList);

  size_t nDim() const;
  size_t nMI() const;

  const MIType* data() const;

  MIType operator()(size_t miIdx, size_t dim) const;
  MIVecElemProxy operator()(size_t miIdx, size_t dim);

  MI operator[](size_t miIdx) const;

  void setMI(size_t idx, const MI& mi);
  void setMI(size_t idx, const std::vector<MIType>& mi);

  bool isDownwardsClosed() const;

  MIVec downwardsClosure() const;

  const std::shared_ptr<MI> componentWiseMax() const;

  const std::shared_ptr<std::vector<size_t>> paretoMaxima(bool isDownwardsClosed = false) const;

  const std::shared_ptr<misc::MIVecLookup> lookup() const;

  void clearCachedValues() const;

 private:
  const size_t nDim_;
  size_t nMI_;

  std::vector<MIType> data_;  // AoS: [idx][dim]

  mutable bool cacheCleared;
  mutable std::shared_ptr<MI> componentWiseMax_;
  mutable std::shared_ptr<std::vector<size_t>> paretoMaxima_;
  mutable std::shared_ptr<misc::MIVecLookup> lookup_;
};

}  // namespace combigrid
}  // namespace sgpp

#endif