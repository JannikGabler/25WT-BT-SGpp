/**
 * @file multiindex_vector_lookup.hpp
 * @brief Hash-based reverse lookup mapping multi-indices stored in an
 * @c MIVec back to their slot index.
 */
#ifndef COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP
#define COMBIGRID_MISC_MULTIINDEX_VECTOR_LOOKUP_HPP

#include <sgpp/combigrid/miscellaneous/multiindex_lookup_equal.hpp>
#include <sgpp/combigrid/miscellaneous/multiindex_vector_lookup_hash.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

namespace misc {

/**
 * @brief Hash-based @f$O(1)@f$ reverse lookup over an @c MIVec.
 *
 * The lookup keeps an @c std::unordered_map keyed by raw pointers into
 * the @c MIVec storage; the hash and equality functors compare the
 * pointed-to multi-indices by value. This avoids materializing each
 * multi-index as an independent @c MI object.
 *
 * @tparam T Multi-index element type.
 *
 * @note The lookup keeps a reference to the underlying @c MIVec; the
 * @c MIVec must outlive any @c MIVecLookup that refers to it, and any
 * mutation of the @c MIVec invalidates the lookup.
 */
template <typename T>
class MIVecLookup {
 public:
  /**
   * @brief Builds the lookup from @p miVec.
   * @param miVec Source vector.
   */
  MIVecLookup<T>(const MIVec<T>& miVec)
      : miVec((miVec)),
        map(miVec.nMI(), MIVecLookupHash<T>(miVec.nDim()), MIVecLookupEqual<T>(miVec.nDim())) {
    const T* dataPtr = miVec.data();

    for (size_t miIdx = 0; miIdx < miVec.nMI(); miIdx++) {
      map.emplace(dataPtr, miIdx);
      dataPtr += miVec.nDim();
    }
  }

  /**
   * @brief Returns @c true iff @p mi is contained in the underlying @c MIVec.
   * @param mi Multi-index to query.
   */
  bool contains(const MI<T>& mi) const { return find(mi) < miVec.nMI(); }

  /**
   * @brief Returns the slot index of @p mi, or @c miVec.nMI() if not present.
   * @param mi Multi-index to query.
   */
  size_t find(const MI<T>& mi) const {
    if (mi.size() != miVec.nDim()) {
      return miVec.nMI();
    }

    const auto entry = map.find(mi.data());

    if (entry == map.end()) {
      return miVec.nMI();
    }

    return entry->second;
  }

 private:
  const MIVec<T>& miVec;  ///< Reference to the indexed storage.
  /// @brief Hash map from raw pointers (into @c miVec storage) to slot indices.
  std::unordered_map<const T*, size_t, MIVecLookupHash<T>, MIVecLookupEqual<T>> map;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif
