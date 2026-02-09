#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/globaldef.hpp>
#include <vector>
#include "sgpp/base/datatypes/DataVector.hpp"

namespace sgpp {
namespace combigrid {

class TensorGrid {
 public:
  TensorGrid(const MI& nGPPerDim);

  TensorGrid(const MI& nGPPerDim, const std::vector<base::DataVector>& gridPoints);

  size_t nDim() const;

  const base::DataVector& getGridPoint(size_t idx) const;

  const base::DataVector& getGridPoint(const MI& mi) const;

  const std::vector<base::DataVector>& getGridPoints() const;

  void setGridPoint(const size_t idx, base::DataVector gp);

  void setGridPoint(const MI& mi, base::DataVector gp);

  //   bool containsABoundary();

 private:
  const MI nGPPerDim;
  std::vector<base::DataVector> gridPoints;  // row-major layout
};

}  // namespace combigrid
}  // namespace sgpp

#endif