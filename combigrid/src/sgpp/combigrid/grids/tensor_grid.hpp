#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

namespace sgpp {
namespace combigrid {

/*
Fixed nDim
*/
class TensorGrid {
 public:
  /**********
  Constructor
  **********/
  TensorGrid() noexcept;

  TensorGrid(const GPMI& nGPPerDim) noexcept;

  TensorGrid(const GPMI& nGPPerDim, base::DataVector&& nodesPerDim) noexcept;

  /*****
  Getter
  *****/
  size_t nDim() const;

  size_t nGP() const;

  const GPMI& getGPCntPerDim() const;

  void getGridPoint(size_t idx, base::DataVector& out) const;

  /*
  Requires allocations
  */
  // base::DataVector getGridPoint(size_t idx) const;

  void getGridPoint(const GPMI& mi, base::DataVector& out) const;

  // base::DataVector getGridPoint(const GPMI& mi) const;

  void getGridPointAndMI(size_t idx, base::DataVector& outGP, GPMI& outMI) const;

  /*****
  Setter
  *****/
  /*
  May invoke an extension of the internal vector if there is no left over capacity.
  */
  // void setGridPoint(const size_t idx, const base::DataVector& gp);

  // void setGridPoint(const GPMI& mi, const base::DataVector& gp);

  //   bool containsABoundary();

  /********
  Operators
  ********/
  // base::DataVector operator[](size_t idx) const;

  // double operator()(size_t idx, size_t dim) const;
  // double& operator()(size_t idx, size_t dim);

  bool operator==(const TensorGrid& other) const;

 private:
  size_t nGP_;     // Fixed (not const because this will delete the copy assig. constr.)
  GPMI nGPPerDim;  // Fixed (not const because this will delete the copy assig. constr.)
  base::DataVector nodesPerDim;  // flattened std::vector<base::DataVector>
};

}  // namespace combigrid
}  // namespace sgpp

#endif