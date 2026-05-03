/**
 * @file multiindex_vector_element_proxy.hpp
 * @brief Proxy returned by @c MIVec::operator() for mutable element access.
 */
#ifndef COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP
#define COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  ///< Forward declaration to avoid circular includes.

/**
 * @brief Write-through proxy referring to a single component of an @c MIVec.
 *
 * Assigning a value to the proxy writes through to the underlying storage
 * and invalidates the cached derived properties of the parent @c MIVec.
 * The proxy is implicitly convertible to @c T for read access.
 *
 * @tparam T Element type of the parent @c MIVec.
 */
template <typename T>
class MIVecElemProxy {
 public:
  /**
   * @brief Constructs a proxy referring to @p ref inside @p miVec.
   *
   * @param miVec Owning @c MIVec used for cache invalidation on writes.
   * @param ref   Reference to the storage cell this proxy targets.
   */
  MIVecElemProxy<T>(const MIVec<T>& miVec, T& ref) : miVec(miVec), ref(ref) {}

  /**
   * @brief Writes @p value into the referenced cell and invalidates the
   * cached derived properties of the parent @c MIVec.
   *
   * @param value New value for the referenced cell.
   * @return Reference to this proxy.
   */
  MIVecElemProxy<T>& operator=(const T value) {
    miVec.clearCachedValues();
    ref = value;
    return *this;
  }

  /// @brief Implicit conversion to the underlying value (read access).
  operator T() const { return ref; }

 private:
  const MIVec<T>& miVec;  ///< Owning @c MIVec used to invalidate caches on writes.
  T& ref;                 ///< Reference to the targeted cell in @c MIVec storage.
};

}  // namespace combigrid
}  // namespace sgpp

#endif
