#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

/*
Implements f(\vec x) = \prod_{i=1}^d |sin(3 * pi * x[i])|
*/
inline double absSinFunction(const DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++) {
    result *= std::sin(3 * M_PI * x[dim]);
  }

  return std::abs(result);
}

double analyticalSolution(const size_t nDim) { return std::pow(2 / M_PI, nDim); }

double perform(const size_t nDim, const LvlType maxLvl) {
  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(absSinFunction);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  CompleteSGGenInstr genInstr(maxLvl, nDim);

  // Use equidistant nodes
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());
  // Grid points on the boundary are on one level lower than the main axis
  genInstr.setBoundaryLevelOffset(999);

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Apply Quadrature === */
  return quadrature(sourceFunc, sg);
}

int main() {
  for (size_t nDim = 1; nDim <= 3; nDim++) {
    const double exactSolution = analyticalSolution(nDim);

    std::cout << nDim << std::scientific << std::setprecision(2);

    for (LvlType maxLvl = 0; maxLvl <= 10; maxLvl += 2) {
      const double result = perform(nDim, maxLvl);
      const double absError = std::abs(exactSolution - result);
      const double relError = absError / std::abs(exactSolution);
      std::cout << " & " << relError;
    }

    std::cout << " & " << std::defaultfloat << std::setprecision(6) << exactSolution << std::endl;
  }

  return 0;
}
