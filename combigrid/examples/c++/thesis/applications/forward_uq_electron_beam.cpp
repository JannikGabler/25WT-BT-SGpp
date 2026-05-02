#include <cmath>
#include <ios>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/benchmarking/benchmarker.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

/*
Implements f(\vec x) = \prod_{i=1}^d (4 * (1 - x[i])^3 * e^{-x[i]/d})
*/
inline double integrant(const DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++) {
    result *= 4 * std::pow(1 - x[dim], 3);
    result *= std::exp(-x[dim] / static_cast<double>(x.size()));
  }

  return result;
}

double analyticalSolution(const size_t nDim) {
  const double d2 = static_cast<double>(nDim * nDim);
  const double d3 = static_cast<double>(nDim * nDim * nDim);
  const double d4 = static_cast<double>(nDim * nDim * nDim * nDim);

  const double v = static_cast<double>(nDim) - 3 * d2 + 6 * d3 - 6 * d4 +
                   6 * d4 * std::exp(-1.0 / static_cast<double>(nDim));

  return std::pow(4 * v, nDim);
}

double perform(const size_t nDim, const LvlType maxLvl) {
  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(integrant);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  FullSGGenInstr genInstr(maxLvl, nDim);

  // Use equidistant nodes
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Apply Quadrature === */
  return quadrature(sourceFunc, sg);
}

int main() {
  for (size_t nDim = 1; nDim <= 8; nDim *= 2) {
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
