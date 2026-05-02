#include <cmath>
#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double func(const DataVector& point) {
  double result = 1;

  for (size_t dim = 0; dim < point.size(); dim++) {
    result *= std::cos(4 * M_PI * point[dim]);
  }

  return result;
}

int main() {
  const SourceFunc sourceFunc(func);

  FullSGGenInstr genInstr(10, 3);
  genInstr.setBounds({0, 1});
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryIndexOffset(0);

  const SparseGrid sg(genInstr);

  const DataVector point{0.9173, 0.13494, 0.4184};
  const double result = interpolate(sourceFunc, point, sg);

  std::cout << "Result: " << result << std::endl;
  std::cout << "Expected: " << func(point) << std::endl;

  return 0;
}