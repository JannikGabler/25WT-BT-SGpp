// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/deltas_sparse_grid_quadrature_index.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>

namespace sgpp {
namespace combigrid {
namespace misc {

DSGQuadIndex::DSGQuadIndex(const DeltasSparseGrid& deltasSparseGrid)
    : weights1D_(tools::dsg_quadrature::comp1DWeights(deltasSparseGrid)),
      coeffTrie_(deltasSparseGrid.getLevels().getView(), deltasSparseGrid.getCoefficients()) {}

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp
