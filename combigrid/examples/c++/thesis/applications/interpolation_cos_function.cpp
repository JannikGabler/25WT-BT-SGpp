#include <cmath>
#include <ios>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

/*
Implements f(\vec x) = \prod_{i=1}^d cos(x[i])
*/
inline double cosFunction(const DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++) {
    result *= std::cos(x[dim]);
  }

  return result;
}

double perform(const size_t nDim, const LvlType maxLvl, const DataVector& point) {
  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(cosFunction);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  CompleteSGGenInstr genInstr(maxLvl, nDim);

  // Use Chebyshev nodes of the second kind
  genInstr.setNodeGenFunc(getSecondTypeChebyshevNodeGenFunc());
  // Change the domain to [-2 * pi, 2 * pi]^d
  genInstr.setDomain({-2 * M_PI, 2 * M_PI});

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Apply Quadrature === */
  return interpolate(sourceFunc, point, sg);
}

int main() {
  std::cout << std::scientific << std::setprecision(2);

  for (size_t nDim = 1; nDim <= 3; nDim++) {
    const DataVector point(nDim, M_PI / 2);

    std::cout << nDim;

    for (LvlType maxLvl = 0; maxLvl <= 10; maxLvl += 2) {
      std::cout << " & " << perform(nDim, maxLvl, point);
    }

    std::cout << " & " << cosFunction(point) << std::endl;
  }

  return 0;
}
