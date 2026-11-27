#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double func(const DataVector& point) { return -13.7; }

int main() {
  const SourceFunc sourceFunc(func);

  CompleteSGGenInstr genInstr(16, 2);
  genInstr.setDomain({-1, 1});
  genInstr.setNodeFamily(getClenshawCurtisNodes());
  genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(2);

  const SparseGrid sg(genInstr);

  const DataVector point{0.9173, 0.13494};
  const double result = interpolate(sourceFunc, point, sg);

  std::cout << "Result: " << result << std::endl;
  return 0;
}