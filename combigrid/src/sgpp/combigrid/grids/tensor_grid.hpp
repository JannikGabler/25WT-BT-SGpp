#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

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
  TensorGrid();

  TensorGrid(const GPMI& nGPPerDim);

  TensorGrid(const GPMI& nGPPerDim, const std::vector<base::DataVector>& gridPoints);

  /*****
  Getter
  *****/
  size_t nDim() const;

  size_t nGP() const;

  base::DataVector getGridPoint(size_t idx) const;

  base::DataVector getGridPoint(const GPMI& mi) const;

  /*****
  Setter
  *****/
  /*
  May invoke an extension of the internal vector if there is no left over capacity.
  */
  void setGridPoint(const size_t idx, const base::DataVector& gp);

  void setGridPoint(const GPMI& mi, const base::DataVector& gp);

  //   bool containsABoundary();

  /*******
  Operator
  *******/
  base::DataVector operator[](size_t idx) const;

  double operator()(size_t idx, size_t dim) const;
  double& operator()(size_t idx, size_t dim);

 private:
  size_t nGP_;                  // Fixed (not const because this will delete the copy assig constr)
  GPMI nGPPerDim;               // Fixed (not const because this will delete the copy assig constr)
  base::DataVector gridPoints;  // flattened vector, row-major layout
};

}  // namespace combigrid
}  // namespace sgpp

#endif