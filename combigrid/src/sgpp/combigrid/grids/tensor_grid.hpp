#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <sgpp/base/datatypes/DataMatrix.hpp>
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

  /*
  Requires a copy
  */
  TensorGrid(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim) noexcept;

  /*
  Moves the arguments
  */
  TensorGrid(GPMI&& nGPPerDim, base::DataVector&& nodesPerDim) noexcept;

  /*****
  Getter
  *****/
  size_t nDim() const;

  size_t nGP() const;

  const GPMI& getGPCntPerDim() const;

  const base::DataVector& getNodesPerDim() const;

  /*
  In-place variant
  */
  void getGridPoint(size_t idx, base::DataVector& out) const;

  /*
  Out-of-place variant (might be inefficient)
  */
  base::DataVector getGridPoint(size_t idx) const;

  /*
  In-place variant
  */
  void getGridPoint(const GPMI& mi, base::DataVector& out) const;

  /*
  Out-of-place variant (might be inefficient)
  */
  base::DataVector getGridPoint(const GPMI& mi) const;

  /*
  In-place variant
  */
  void getGridPointAndMI(size_t idx, base::DataVector& outGP, GPMI& outMI) const;

  /*
  Out-of-place variant (might be inefficient)
  */
  std::pair<base::DataVector, GPMI> getGridPointAndMI(size_t idx) const;

  /*
  In-place variant
  Not optimized (TODO)
  */
  void getGridPoints(base::DataMatrix& out) const;

  /*
  Out-of-place variant (might be inefficient)
  */
  base::DataMatrix getGridPoints() const;

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