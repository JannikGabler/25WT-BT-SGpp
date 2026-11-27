// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

// ThreadSanitizer driver of the dimension-adaptive refinement: both schedules, evaluation ahead,
// a cached source function, an exception of the source function; 8 threads.

#include <omp.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <memory>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/refinement/dimension_adaptive_refinement.hpp>
#include <sgpp/combigrid/refinement/targets/global_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/quadrature_refinement_target.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <stdexcept>

namespace cg = sgpp::combigrid;

static double gauss(const sgpp::base::DataVector& x) {
  double sum = 0.0;
  for (size_t k = 0; k < x.size(); k++) {
    const double t = 4.0 / static_cast<double>((k + 1) * (k + 1)) * (x[k] - 0.5);
    sum += t * t;
  }
  return std::exp(-sum);
}

static std::unique_ptr<cg::SparseGrid> startGrid(size_t nDim) {
  cg::CompleteSGGenInstr instr(0, nDim);
  instr.setNodeGenFunc(cg::getClenshawCurtisNodeGenFunc());
  instr.setBoundaryLevelOffset(1);
  return std::make_unique<cg::SparseGrid>(instr);
}

int main() {
  omp_set_num_threads(8);
  const cg::SourceFunc f(gauss);
  for (const auto schedule : {cg::RefinementSchedule::Synchronized, cg::RefinementSchedule::Asynchronous}) {
    for (const size_t window : {size_t{0}, size_t{16}}) {
      std::unique_ptr<cg::SparseGrid> sgPtr = startGrid(8);
      cg::SparseGrid& sg = *sgPtr;
      cg::QuadratureRefinementTarget target(f);
      cg::RefinementOptions options;
      options.maxGridPoints = 20000;
      options.schedule = schedule;
      options.speculationWindow = window;
      cg::DimensionAdaptiveRefinement refinement(sg, target, options);
      refinement.refine();
      refinement.step();
      refinement.refine();
      std::printf("schedule %d window %zu: %zu TGs, estimate %.15g\n", static_cast<int>(schedule), window,
                  sg.nTG(), refinement.getEstimate()[0]);
    }
  }
  {
    const cg::CachedSourceFunc cached(gauss);
    std::unique_ptr<cg::SparseGrid> sgPtr = startGrid(6);
    cg::SparseGrid& sg = *sgPtr;
    sgpp::base::DataMatrix points(4, 6, 0.3);
    cg::GlobalInterpolationRefinementTarget target(cached, points);
    cg::RefinementOptions options;
    options.maxGridPoints = 5000;
    cg::refine(sg, target, options);
    std::printf("cached interpolation: %zu TGs\n", sg.nTG());
  }
  {
    std::atomic<int> n{0};
    const cg::SourceFunc failing([&](const sgpp::base::DataVector& x) {
      if (++n == 500) throw std::runtime_error("expected");
      return gauss(x);
    });
    std::unique_ptr<cg::SparseGrid> sgPtr = startGrid(8);
    cg::SparseGrid& sg = *sgPtr;
    cg::QuadratureRefinementTarget target(failing);
    cg::RefinementOptions options;
    options.maxGridPoints = 20000;
    try {
      cg::refine(sg, target, options);
    } catch (const std::runtime_error&) {
      std::printf("exception rethrown, %zu TGs\n", sg.nTG());
    }
  }
  return 0;
}
