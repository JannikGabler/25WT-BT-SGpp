#include <cmath>
#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include "sgpp/combigrid/type_defs.hpp"

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

double func(const DataVector& point) {
  double result = 1;

  for (size_t dim = 1; dim < point.size(); dim++) {
    result *= std::cos(4 * M_PI * point[dim]);
  }

  return result;
}

int main() {
  const SourceFunc sourceFunc(func);

  const LvlMIVec mis{{0, 5, 0}, {0, 4, 1}, {0, 3, 2}, {0, 2, 3}, {0, 1, 4}, {0, 0, 5}};

  MIVecSGGenInstr genInstr(mis);
  genInstr.setDomain({0, 1});
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(0);

  const SparseGrid sg(genInstr);

  const double result = quadrature(sourceFunc, sg);

  std::cout << "Result: " << result << std::endl;
  return 0;
}