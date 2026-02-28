#ifndef COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP
#define COMBIGRID_MULTIINDEX_VECTOR__ELEMENT_PROXY_HPP

namespace sgpp {
namespace combigrid {

template <typename T>
class MIVec;  // Forward declaration to avoid circular includes

template <typename T>
class MIVecElemProxy {
 public:
  MIVecElemProxy<T>(const MIVec<T>& miVec, T& ref) : miVec(miVec), ref(ref) {}

  MIVecElemProxy<T>& operator=(const T value) {
    miVec.clearCachedValues();
    ref = value;
    return *this;
  }

  operator T() const { return ref; }

 private:
  const MIVec<T>& miVec;
  T& ref;
};

}  // namespace combigrid
}  // namespace sgpp

#endif