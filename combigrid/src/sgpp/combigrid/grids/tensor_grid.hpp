#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

class TensorGrid {
 public:
  TensorGrid(const std::vector<size_t>& nGPPerDim);

  TensorGrid(const std::vector<size_t>& nGPPerDim, const std::vector<base::DataVector>& gridPoints);

  size_t nDim() const;

  size_t nGP() const;

  base::DataVector getGridPoint(size_t idx) const;

  base::DataVector getGridPoint(const std::vector<size_t>& mi) const;

  // const std::vector<base::DataVector>& getGridPoints() const;

  void setGridPoint(const size_t idx, const base::DataVector& gp);

  void setGridPoint(const std::vector<size_t>& mi,
                    const base::DataVector& gp);  // TODO: Is MI a good data type?

  //   bool containsABoundary();

  /********
  Operators
  ********/
  base::DataVector operator[](size_t idx) const;

  double operator()(size_t idx, size_t dim) const;
  double& operator()(size_t idx, size_t dim);

 private:
  const size_t nGP_;
  const std::vector<size_t> nGPPerDim;
  base::DataVector gridPoints;  // flattened vector, row-major layout
};

}  // namespace combigrid
}  // namespace sgpp

#endif