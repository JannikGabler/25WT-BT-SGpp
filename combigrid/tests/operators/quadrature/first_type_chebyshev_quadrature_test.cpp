// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file first_type_chebyshev_quadrature_test.cpp
 * @brief Boost.Test cases for the sparse-grid quadrature with first-kind Chebyshev nodes
 * (exactness of the combination technique) and for node families without a quadrature rule
 * (a catchable exception at any thread count).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/not_implemented_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/default_functions/equidistant_node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::GPCntType;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMI;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::MIVecSGGenInstr;
using sgpp::combigrid::QuadratureOptions;
using sgpp::combigrid::QuadRule;
using sgpp::combigrid::SGGenInstr;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::VectorSourceFunc;

namespace {

/// Boundary level offset without boundary nodes at the levels used here.
constexpr LvlType NO_BOUNDARY = 20;

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

/// The degree up to which the 1D rule of level @p lvl is exact (doubling: @f$2^{\ell+1} - 1@f$
/// inner nodes, plus 0 and 1 from the offset on; interpolatory on n nodes: degree n - 1).
size_t exactDegree(const LvlType lvl, const LvlType boundaryOffset) {
  const size_t nInner = (size_t{2} << lvl) - 1;
  return (lvl >= boundaryOffset) ? nInner + 1 : nInner - 1;
}

/// The 1D factor @f$T_k(t) + T_{k/2}(t) / 2 + 1/4@f$ (degree k) at @f$t \in [-1, 1]@f$.
double factor(const size_t k, const double t) {
  const double theta = std::acos(std::max(-1.0, std::min(1.0, t)));
  return std::cos(static_cast<double>(k) * theta) +
         0.5 * std::cos(static_cast<double>(k / 2) * theta) + 0.25;
}

/// The mean of @ref factor over [-1, 1] (the mean of T_k is 1 / (1 - k^2) for even k, else 0).
double meanOfFactor(const size_t k) {
  const auto meanOfT = [](const size_t j) {
    const double jDouble = static_cast<double>(j);
    return (j % 2 == 1) ? 0.0 : 1.0 / (1.0 - jDouble * jDouble);
  };
  return meanOfT(k) + 0.5 * meanOfT(k / 2) + 0.25;
}

/// Checks that the quadrature on @p genInstr's grid integrates the product of the factors of
/// degrees exactDegree(ell[j]) exactly, i.e. the polynomial of the exactness space of the tensor
/// grid @p ell (which must be in the grid's downward closed set).
void checkExact(SGGenInstr& genInstr, const LvlMI& ell, const LvlType boundary,
                const bool shifted) {
  const size_t nDim = ell.size();
  genInstr.setNodeGenFunc(sgpp::combigrid::getFirstTypeChebyshevNodeGenFunc());
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(boundary);
  const HyperCubeArea domain = shifted ? shiftedDomain(nDim) : HyperCubeArea(nDim, {0.0, 1.0});
  genInstr.setDomain(domain);
  const SparseGrid sg(genInstr);

  std::vector<size_t> degrees(nDim);
  double exact = 1.0;
  for (size_t j = 0; j < nDim; j++) {
    degrees[j] = exactDegree(ell[j], boundary);
    exact *= (domain[j].second - domain[j].first) * meanOfFactor(degrees[j]);
  }
  const auto f = [&](const DataVector& x) {
    double value = 1.0;
    for (size_t j = 0; j < nDim; j++) {
      const double t = 2.0 * (x[j] - domain[j].first) / (domain[j].second - domain[j].first) - 1;
      value *= factor(degrees[j], t);
    }
    return value;
  };
  const SourceFunc sourceFunc(f);
  const VectorSourceFunc vectorFunc(
      [&](const DataVector& x, std::span<double> out) {
        out[0] = f(x);
        out[1] = 2.0 * f(x);
      },
      2);

  const double tol = 1e-12 * std::max(1.0, std::abs(exact));
  BOOST_CHECK_SMALL(sgpp::combigrid::quadrature(sourceFunc, sg) - exact, tol);
  BOOST_CHECK_SMALL(sgpp::combigrid::textbookQuadrature(sourceFunc, sg) - exact, tol);
  const DataVector vectorResult = sgpp::combigrid::quadrature(vectorFunc, sg);
  BOOST_CHECK_SMALL(vectorResult[0] - exact, tol);
  BOOST_CHECK_SMALL(vectorResult[1] - 2.0 * exact, 2.0 * tol);
}

/// Equidistant nodes whose quadrature rule is missing (a user-defined family without a rule).
class NoRuleNodeGenFunc : public sgpp::combigrid::node_gen_funcs::EquidistantNodeGenFunc {
 public:
  QuadRule* getQuadRule(GPCntType /*nNodes*/, bool) const override {
    throw sgpp::base::not_implemented_exception("No quadrature rule");
  }
};

/// Runs @p body with @p nThreads OpenMP threads (restores the previous number afterwards).
template <typename Body>
void withThreads(const int nThreads, const Body& body) {
  const int before = omp_get_max_threads();
  omp_set_num_threads(nThreads);
  body();
  omp_set_num_threads(before);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(first_type_chebyshev_quadrature)

BOOST_AUTO_TEST_CASE(complete_grids_are_exact_on_their_polynomial_spaces) {
  // d 1-4, levels up to 6 / 5 / 4 / 3, every tensor grid of the largest level sum (anisotropic
  // ones: all mass in one dimension, the rest spread), boundary nodes always / from level 1 / never
  const std::vector<LvlType> maxLevel{6, 5, 4, 3};
  for (size_t nDim = 1; nDim <= 4; nDim++) {
    for (LvlType level = 0; level <= maxLevel[nDim - 1]; level++) {
      for (const LvlType boundary : {LvlType{0}, LvlType{1}, NO_BOUNDARY}) {
        for (const bool shifted : {false, true}) {
          for (size_t heavyDim = 0; heavyDim < nDim; heavyDim++) {
            LvlMI ell(nDim, 0);
            ell[heavyDim] = level;
            CompleteSGGenInstr genInstr(level, nDim);
            checkExact(genInstr, ell, boundary, shifted);
            if (nDim >= 2 && level >= 1) {  // Mass split between two dimensions
              LvlMI split(nDim, 0);
              split[heavyDim] = level - level / 2;
              split[(heavyDim + 1) % nDim] = level / 2;
              CompleteSGGenInstr splitInstr(level, nDim);
              checkExact(splitInstr, split, boundary, shifted);
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(anisotropic_grids_are_exact_on_their_polynomial_spaces) {
  LvlMIVec miVec(std::vector<LvlMI>{{4, 0, 1}, {1, 2, 0}, {0, 1, 3}});
  for (const LvlType boundary : {LvlType{0}, LvlType{1}, NO_BOUNDARY}) {
    for (const LvlMI& ell : {LvlMI{4, 0, 1}, LvlMI{1, 2, 0}, LvlMI{0, 1, 3}, LvlMI{1, 1, 0}}) {
      MIVecSGGenInstr genInstr(miVec);
      checkExact(genInstr, ell, boundary, true);
    }
  }
}

BOOST_AUTO_TEST_CASE(missing_rule_throws_catchable_exception_at_any_thread_count) {
  NoRuleNodeGenFunc noRule;
  CompleteSGGenInstr genInstr(4, 3);
  genInstr.setNodeGenFunc(&noRule);
  genInstr.setLvl2GPCntFunc(sgpp::combigrid::doublingLvl2GPCntFunction);
  genInstr.setBoundaryLevelOffset(1);
  const SparseGrid sg(genInstr);
  const SourceFunc f([](const DataVector& x) { return x[0]; });
  const VectorSourceFunc vf([](const DataVector& x, std::span<double> out) { out[0] = x[1]; }, 1);
  QuadratureOptions cached;
  cached.useCrossCallOperatorDataCache = true;

  for (const int nThreads : {1, 16}) {
    withThreads(nThreads, [&]() {
      BOOST_CHECK_THROW(sgpp::combigrid::quadrature(f, sg), sgpp::base::not_implemented_exception);
      BOOST_CHECK_THROW(sgpp::combigrid::quadrature(vf, sg), sgpp::base::not_implemented_exception);
      for (int repeat = 0; repeat < 2; repeat++) {  // A failed build is not cached
        BOOST_CHECK_THROW(sgpp::combigrid::quadrature(f, sg, cached),
                          sgpp::base::not_implemented_exception);
        BOOST_CHECK(sg.getCrossCallOperatorDataCacheEntries().empty());
      }
      BOOST_CHECK_THROW(sgpp::combigrid::textbookQuadrature(f, sg),
                        sgpp::base::not_implemented_exception);
    });
  }
}

BOOST_AUTO_TEST_SUITE_END()
