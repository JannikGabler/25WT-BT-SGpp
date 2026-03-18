#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double func(const DataVector& point) { return -13.7; }

int main() {
  const SourceFunc sourceFunc(func);

  FullSGGenInstr genInstr(16, 2);
  genInstr.setBounds({-1, 1});
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(linearLvl2GPCntFunction);
  genInstr.setBoundaryIndexOffset(2);

  const SparseGrid sg(genInstr);

  const double result = quadrature(sourceFunc, sg);

  std::cout << "Result: " << result << std::endl;
  return 0;
}