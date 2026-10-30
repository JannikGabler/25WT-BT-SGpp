// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_quadrature_test.cpp
 * @brief Boost.Test cases for the sparse-grid quadrature of vector-valued source functions
 * (@c VectorSourceFunc): output k equals the scalar quadrature of output k up to round-off.
 *
 * The vector and the scalar kernels perform the same operations per output, but the compiler may
 * contract them differently to FMAs (memory vs. register accumulators), and the thread results
 * are combined in a different order. The test integrands are positive and all quadrature weights
 * are positive, so the sums of a tensor grid do not cancel and round-off is relative to them.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cmath>
#include <limits>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Allowed distance between vector and scalar results, in units of eps times the scale.
constexpr double ULP_TOL = 16.0;
constexpr double EPS = std::numeric_limits<double>::epsilon();

/// Output k: exp(-|x - c_k|^2) * (1 + 0.5 k x_0) + 1 + k with c_k = 0.2 + 0.15 k (positive on
/// the test domains, not separable).
double component(const DataVector& x, const size_t k) {
  const double center = 0.2 + 0.15 * static_cast<double>(k);
  double sqDist = 0.0;
  for (const double v : x) sqDist += (v - center) * (v - center);
  const double kd = static_cast<double>(k);
  return std::exp(-sqDist) * (1.0 + 0.5 * kd * x[0]) + 1.0 + kd;
}

void components(const DataVector& x, std::span<double> out) {
  for (size_t k = 0; k < out.size(); k++) out[k] = component(x, k);
}

/// Dimension i: [-0.5 + 0.1 i, 1.0 + 0.2 i].
HyperCubeArea shiftedDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);
  for (size_t i = 0; i < nDim; i++) {
    domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
  }
  return domain;
}

/// Anisotropic MIs (closure computed by MIVecSGGenInstr): 3 e_0, (1, ..., 1), 2 e_{d-1}.
LvlMIVec anisotropicMIs(const size_t nDim) {
  LvlMI first(nDim, 0), ones(nDim, 1), last(nDim, 0);
  first[0] = 3;
  last[nDim - 1] = 2;
  return LvlMIVec(std::vector<LvlMI>{first, ones, last});
}

void configure(SGGenInstr& genInstr, const std::string& nodes, const Lvl2GPCntFunc lvl2GPCntFunc,
               const LvlType boundary, const bool unitDomain) {
  const size_t nDim = genInstr.nDim();
  for (size_t dim = 0; dim < nDim; dim++) {
    const bool equi = nodes == "equidistant" || (nodes == "mixed" && dim % 2 == 0);
    genInstr.setNodeGenFuncForDim(
        equi ? getEquidistantNodeGenFunc() : getClenshawCurtisNodeGenFunc(), dim);
  }
  genInstr.setLvl2GPCntFunc(lvl2GPCntFunc);
  genInstr.setBoundaryLevelOffset(boundary);
  genInstr.setDomain(unitDomain ? HyperCubeArea(nDim, {0.0, 1.0}) : shiftedDomain(nDim));
}

/**
 * Compares the vector processTensorGrid with the scalar one of every output, tensor grid by
 * tensor grid (same points in the same order, results within round-off), and returns the scale
 * of the combined results: per output, volume * sum of |coefficient| * estimate of the tensor
 * grid.
 */
std::vector<double> checkTensorGrids(const SparseGrid& sg, const size_t nOutputs) {
  const misc::QuadratureContext context{.sparseGrid = sg,
                                        .domain = sg.getGenInstr().getDomain(),
                                        .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
  misc::QuadratureScratch scalarScratch(sg.nDim(), sg.getMaxTGSumOverGPCntsPerDim());
  misc::QuadratureScratch vectorScratch(sg.nDim(), sg.getMaxTGSumOverGPCntsPerDim(), nOutputs);

  std::vector<DataVector> vectorPoints, scalarPoints;
  const VectorSourceFunc vectorFunc(
      [&vectorPoints, nOutputs](const DataVector& x, std::span<double> out) {
        BOOST_REQUIRE_EQUAL(out.size(), nOutputs);
        vectorPoints.push_back(x);
        components(x, out);
      },
      nOutputs);

  std::vector<double> scales(nOutputs, 0.0);

  for (const TensorGridCTData& tgData : sg) {
    std::vector<double> out(nOutputs, std::numeric_limits<double>::quiet_NaN());
    vectorPoints.clear();
    quadrature_operator::processTensorGrid(context, vectorFunc, tgData.tensorGrid, vectorScratch,
                                           out);
    BOOST_CHECK_EQUAL(vectorPoints.size(), tgData.tensorGrid.nGP());

    for (size_t k = 0; k < nOutputs; k++) {
      const SourceFunc scalarFunc([&scalarPoints, k](const DataVector& x) {
        scalarPoints.push_back(x);
        return component(x, k);
      });
      scalarPoints.clear();
      const double scalar = quadrature_operator::processTensorGrid(
          context, scalarFunc, tgData.tensorGrid, scalarScratch);

      BOOST_CHECK(scalarPoints == vectorPoints);
      BOOST_CHECK_MESSAGE(
          std::abs(out[k] - scalar) <= ULP_TOL * EPS * scalar,
          "tensor grid, output " << k << ": vector " << out[k] << ", scalar " << scalar);
      scales[k] += std::abs(static_cast<double>(tgData.coefficient)) * scalar;
    }
  }

  for (double& scale : scales) scale *= sg.getGenInstr().getVolumeOfDomain();
  return scales;
}

/**
 * Compares both vector quadrature overloads with the scalar quadrature of every output (and the
 * kernels tensor grid by tensor grid, checkTensorGrids).
 */
void checkVectorQuadrature(const SGGenInstr& genInstr, const size_t nOutputs) {
  const SparseGrid sg(genInstr);
  const std::vector<double> scales = checkTensorGrids(sg, nOutputs);
  const VectorSourceFunc vectorFunc(components, nOutputs);

  std::vector<double> viaSpan(nOutputs, std::numeric_limits<double>::quiet_NaN());
  quadrature(vectorFunc, sg, viaSpan);
  const DataVector viaDataVector = quadrature(vectorFunc, sg);
  BOOST_REQUIRE_EQUAL(viaDataVector.size(), nOutputs);

  for (size_t k = 0; k < nOutputs; k++) {
    const double scalar =
        quadrature(SourceFunc([k](const DataVector& x) { return component(x, k); }), sg);
    const double tol = ULP_TOL * EPS * scales[k];

    BOOST_CHECK_MESSAGE(std::abs(viaSpan[k] - scalar) <= tol,
                        "output " << k << ": vector (span) " << viaSpan[k] << ", scalar " << scalar
                                  << ", difference " << viaSpan[k] - scalar);
    BOOST_CHECK_MESSAGE(
        std::abs(viaDataVector[k] - scalar) <= tol,
        "output " << k << ": vector (DataVector) " << viaDataVector[k] << ", scalar " << scalar);
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(vector_quadrature)

BOOST_AUTO_TEST_CASE(Outputs_Equal_Scalar_Quadrature) {
  // Node families with a quadrature rule (first-kind Chebyshev nodes have none), growth
  // functions, boundary level offsets 0 / 1, d = 1, ..., 4, unit and shifted domains, complete
  // (levels 0 and 3: single-point grids at b1 / level 0) and MI-vector grids, m = 1 / 3
  for (const std::string nodes : {"equidistant", "Clenshaw-Curtis", "mixed"}) {
    for (const Lvl2GPCntFunc lvl2GPCntFunc :
         {&doublingLvl2GPCntFunction, &linearLvl2GPCntFunction}) {
      for (const LvlType boundary : {0u, 1u}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (const bool unitDomain : {true, false}) {
            for (const size_t nOutputs : {1u, 3u}) {
              const LvlMIVec mis = anisotropicMIs(nDim);
              CompleteSGGenInstr level0(0, nDim);
              CompleteSGGenInstr level3(3, nDim);
              MIVecSGGenInstr miVec(mis);

              for (SGGenInstr* const genInstr :
                   {static_cast<SGGenInstr*>(&level0), static_cast<SGGenInstr*>(&level3),
                    static_cast<SGGenInstr*>(&miVec)}) {
                configure(*genInstr, nodes, lvl2GPCntFunc, boundary, unitDomain);
                BOOST_TEST_CONTEXT(
                    nodes << " nodes, "
                          << (lvl2GPCntFunc == &doublingLvl2GPCntFunction ? "doubling" : "linear")
                          << " growth, boundary level offset " << boundary << ", nDim " << nDim
                          << (unitDomain ? ", unit" : ", shifted") << " domain, "
                          << (genInstr == &miVec ? "MIVec" : "complete") << ", m = " << nOutputs) {
                  checkVectorQuadrature(*genInstr, nOutputs);
                }
              }
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Zero_Point_Tensor_Grid) {
  // A tensor grid without grid points contributes 0 to every output and does not evaluate the
  // source function
  CompleteSGGenInstr genInstr(2, 3);
  const SparseGrid sg(genInstr);

  size_t nEvaluations = 0;
  const VectorSourceFunc sourceFunc(
      [&nEvaluations](const DataVector&, std::span<double> out) {
        nEvaluations++;
        for (double& value : out) value = 1.0;
      },
      3);
  const misc::QuadratureContext context{.sparseGrid = sg,
                                        .domain = sg.getGenInstr().getDomain(),
                                        .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
  misc::QuadratureScratch scratch(3, sg.getMaxTGSumOverGPCntsPerDim(), 3);

  for (const GPMI& nGPPerDim : {GPMI{3, 0, 5}, GPMI{0, 3, 5}, GPMI{3, 5, 0}}) {
    const TensorGrid tg(nGPPerDim);
    BOOST_REQUIRE_EQUAL(tg.nGP(), 0u);
    std::vector<double> out(3, std::numeric_limits<double>::quiet_NaN());
    quadrature_operator::processTensorGrid(context, sourceFunc, tg, scratch, out);
    for (const double value : out) BOOST_CHECK_EQUAL(value, 0.0);
  }

  BOOST_CHECK_EQUAL(nEvaluations, 0u);
}

BOOST_AUTO_TEST_CASE(Wrong_Result_Size_Is_Rejected) {
  CompleteSGGenInstr genInstr(2, 2);
  const SparseGrid sg(genInstr);
  const VectorSourceFunc sourceFunc(components, 3);

  for (const size_t size : {0u, 2u, 4u}) {
    std::vector<double> result(size);
    BOOST_CHECK_THROW(quadrature(sourceFunc, sg, result), sgpp::base::data_exception);
  }
}

BOOST_AUTO_TEST_SUITE_END()
