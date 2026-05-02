#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double func(const DataVector& point) { return -13.7; }

int main() {
  const SourceFunc sourceFunc(func);

  const CompleteSGGenInstr genInstr(2, 2);

  const SparseGrid sg(genInstr);

  const DataVector point{0.9173, 0.13494};
  const double result = interpolate(sourceFunc, point, sg);

  std::cout << "Result: " << result << std::endl;
  return 0;
}