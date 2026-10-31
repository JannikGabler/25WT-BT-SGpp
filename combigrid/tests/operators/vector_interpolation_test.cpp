// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_interpolation_test.cpp
 * @brief Boost.Test cases for the global (@c interpolate) and the linear (@c interpolateLinear)
 * interpolation of vector-valued source functions (@c VectorSourceFunc): output k equals the
 * scalar interpolation of output k up to round-off; and for the multi-output overload of
 * @c InterpolationMethod::interpolate (default and built-in overrides).
 *
 * The vector kernels perform the same operations per output as the scalar ones, but the
 * compiler may contract them differently to FMAs, and the thread results are combined in a
 * different order. The Lagrange basis functions of the global interpolation have both signs, so
 * round-off is bounded relative to the sum of the absolute terms @f$\sum_i |\ell_i(x) f_i|@f$
 * (computed here with the explicit Lagrange basis). The weights of the linear interpolation are
 * non-negative: there, the bound is the interpolant of @f$|f|@f$.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Allowed distance between vector and scalar results, in units of eps times the scale.
constexpr double ULP_TOL = 16.0;
constexpr double EPS = std::numeric_limits<double>::epsilon();

/// Output k: exp(-|x - c_k|^2) * (1 + 0.5 k x_0) - 0.5 + 0.25 k with c_k = 0.2 + 0.15 k (not
/// separable; output 0 changes its sign on the test domains).
double component(const DataVector& x, const size_t k) {
  const double center = 0.2 + 0.15 * static_cast<double>(k);
  double sqDist = 0.0;
  for (const double v : x) sqDist += (v - center) * (v - center);
  const double kd = static_cast<double>(k);
  return std::exp(-sqDist) * (1.0 + 0.5 * kd * x[0]) - 0.5 + 0.25 * kd;
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

NodeGenFunc* nodeGenFuncByName(const std::string& nodes) {
  if (nodes == "equi") return getEquidistantNodeGenFunc();
  if (nodes == "cc") return getClenshawCurtisNodeGenFunc();
  return getFirstTypeChebyshevNodeGenFunc();
}

/// The generation instruction of a test configuration (complete grid: maximum level 3; else
/// anisotropicMIs, stored in @p miVec, which must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const std::string& nodes, const LvlType boundary,
                                         const bool unitDomain, const size_t nDim, const bool mivec,
                                         LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (mivec) {
    miVec = anisotropicMIs(nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(3, nDim);
  }
  genInstr->setNodeGenFunc(nodeGenFuncByName(nodes));
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(boundary);
  genInstr->setDomain(unitDomain ? HyperCubeArea(nDim, {0.0, 1.0}) : shiftedDomain(nDim));
  return genInstr;
}

/// A generic query point and one whose first coordinate is the domain's midpoint (a node).
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  DataVector generic(nDim), onNode(nDim);
  for (size_t i = 0; i < nDim; i++) {
    const double t = 0.137 + 0.211 * static_cast<double>(i);
    generic[i] = domain[i].first + (t - std::floor(t)) * (domain[i].second - domain[i].first);
    onNode[i] = generic[i];
  }
  onNode[0] = 0.5 * (domain[0].first + domain[0].second);
  return {generic, onNode};
}

/// Lagrange basis function @p j of @p nodes (n of them, starting at @p nodes) at @p x.
double lagrangeBasis(const double* const nodes, const size_t n, const size_t j, const double x) {
  double result = 1.0;
  for (size_t i = 0; i < n; i++) {
    if (i != j) result *= (x - nodes[i]) / (nodes[j] - nodes[i]);
  }
  return result;
}

/**
 * Scale of the global interpolation on one tensor grid: per output, the sum over its grid points
 * of |Lagrange basis function at @p normPoint * value|.
 */
std::vector<double> absLagrangeSums(const TensorGridCTData& tgData, const HyperCubeArea& domain,
                                    const DataVector& normPoint, const size_t nOutputs) {
  const TensorGrid& tg = tgData.tensorGrid;
  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const DataVector& nodesPerDim = tg.getNodesPerDim();
  std::vector<double> sums(nOutputs, 0.0), values(nOutputs);
  DataVector gp(nDim);

  for (size_t idx = 0; idx < tg.nGP(); idx++) {
    double basis = 1.0;
    size_t rest = idx, offset = 0;
    for (size_t dim = 0; dim < nDim; dim++) {
      const size_t n = nGPPerDim[dim];
      basis *= lagrangeBasis(nodesPerDim.data() + offset, n, rest % n, normPoint[dim]);
      rest /= n;
      offset += n;
    }
    tg.getGridPoint(idx, gp);
    tools::denormalizeDataVector(gp, domain);
    components(gp, values);
    for (size_t k = 0; k < nOutputs; k++) sums[k] += std::abs(basis * values[k]);
  }

  return sums;
}

/// SourceFunc of output @p k (@p absolute: of its absolute value).
SourceFunc scalarFunc(const size_t k, const bool absolute = false) {
  return SourceFunc([k, absolute](const DataVector& x) {
    return absolute ? std::abs(component(x, k)) : component(x, k);
  });
}

/// VectorSourceFunc of the first @p nOutputs outputs that records its points in @p points.
VectorSourceFunc recordingVectorFunc(const size_t nOutputs, std::vector<DataVector>& points) {
  return VectorSourceFunc(
      [&points, nOutputs](const DataVector& x, std::span<double> out) {
        BOOST_REQUIRE_EQUAL(out.size(), nOutputs);
        points.push_back(x);
        components(x, out);
      },
      nOutputs);
}

/**
 * Global interpolation: compares the vector kernel with the scalar one of every output, tensor
 * grid by tensor grid (same points in the same order, results within round-off), and returns
 * the scale of the combined results: per output, the sum of |coefficient| * absLagrangeSums.
 */
std::vector<double> checkGlobalTensorGrids(const SparseGrid& sg, const DataVector& normPoint,
                                           const size_t nOutputs) {
  const SGGenInstr& genInstr = sg.getGenInstr();
  std::vector<DataVector> vectorPoints, scalarPoints;
  const VectorSourceFunc vectorFunc = recordingVectorFunc(nOutputs, vectorPoints);
  std::vector<double> scales(nOutputs, 0.0);

  for (const TensorGridCTData& tgData : sg) {
    std::vector<double> out(nOutputs, std::numeric_limits<double>::quiet_NaN());
    vectorPoints.clear();
    tools::global_interpolation::processTensorGrid(vectorFunc, normPoint, tgData, genInstr, out);
    const std::vector<double> tgScales =
        absLagrangeSums(tgData, genInstr.getDomain(), normPoint, nOutputs);

    for (size_t k = 0; k < nOutputs; k++) {
      scalarPoints.clear();
      const SourceFunc fk([&scalarPoints, k](const DataVector& x) {
        scalarPoints.push_back(x);
        return component(x, k);
      });
      const double scalar =
          tools::global_interpolation::processTensorGrid(fk, normPoint, tgData, genInstr);
      BOOST_CHECK_SMALL(out[k] - scalar, ULP_TOL * EPS * tgScales[k] + 1e-300);
      BOOST_REQUIRE_EQUAL(vectorPoints.size(), scalarPoints.size());
      for (size_t i = 0; i < vectorPoints.size(); i++) {
        BOOST_CHECK(vectorPoints[i] == scalarPoints[i]);
      }
      scales[k] += std::abs(tgData.coefficient) * tgScales[k];
    }
  }

  return scales;
}

/**
 * Linear interpolation: as checkGlobalTensorGrids (scale of a tensor grid: the interpolant of
 * |output|, the corner weights being non-negative).
 */
std::vector<double> checkLinearTensorGrids(const SparseGrid& sg, const DataVector& normPoint,
                                           const size_t nOutputs) {
  const SGGenInstr& genInstr = sg.getGenInstr();
  misc::LinInterpScratch scalarScratch(sg.nDim());
  misc::LinInterpScratch vectorScratch(sg.nDim(), nOutputs);
  std::vector<DataVector> vectorPoints, scalarPoints;
  const VectorSourceFunc vectorFunc = recordingVectorFunc(nOutputs, vectorPoints);
  std::vector<double> scales(nOutputs, 0.0);

  for (const TensorGridCTData& tgData : sg) {
    std::vector<double> out(nOutputs, std::numeric_limits<double>::quiet_NaN());
    vectorPoints.clear();
    tools::lin_interpolation::processTensorGrid(vectorFunc, normPoint, tgData, genInstr,
                                                vectorScratch, out);

    for (size_t k = 0; k < nOutputs; k++) {
      scalarPoints.clear();
      const SourceFunc fk([&scalarPoints, k](const DataVector& x) {
        scalarPoints.push_back(x);
        return component(x, k);
      });
      const double scalar = tools::lin_interpolation::processTensorGrid(fk, normPoint, tgData,
                                                                        genInstr, scalarScratch);
      const double tgScale = tools::lin_interpolation::processTensorGrid(
          scalarFunc(k, true), normPoint, tgData, genInstr, scalarScratch);
      BOOST_CHECK_SMALL(out[k] - scalar, ULP_TOL * EPS * tgScale + 1e-300);
      BOOST_REQUIRE_EQUAL(vectorPoints.size(), scalarPoints.size());
      for (size_t i = 0; i < vectorPoints.size(); i++) {
        BOOST_CHECK(vectorPoints[i] == scalarPoints[i]);
      }
      scales[k] += std::abs(tgData.coefficient) * tgScale;
    }
  }

  return scales;
}

/// Runs @p check for all configurations (nodes / boundary / domain / d / grid / m / point).
template <typename Check>
void forAllConfigs(const std::vector<std::string>& nodeNames, const Check check) {
  for (const std::string& nodes : nodeNames) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const bool unitDomain : {true, false}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (const bool mivec : {false, true}) {
            LvlMIVec miVec(nDim, 0);
            const std::unique_ptr<SGGenInstr> genInstr =
                makeGenInstr(nodes, boundary, unitDomain, nDim, mivec, miVec);
            const SparseGrid sg(*genInstr);
            for (const size_t nOutputs : {1, 3}) {
              for (const DataVector& point : testPoints(genInstr->getDomain())) {
                BOOST_TEST_CONTEXT(nodes << " b" << boundary << " unit " << unitDomain << " d"
                                         << nDim << " mivec " << mivec << " m" << nOutputs
                                         << " point " << point.toString()) {
                  check(sg, point, nOutputs);
                }
              }
            }
          }
        }
      }
    }
  }
}

/// Compares both vector overloads of an operator with its scalar overload for every output.
template <typename ScalarOp, typename SpanOp, typename VectorOp>
void checkOperator(const SparseGrid& sg, const DataVector& point, const size_t nOutputs,
                   const std::vector<double>& scales, const ScalarOp scalarOp, const SpanOp spanOp,
                   const VectorOp vectorOp) {
  const VectorSourceFunc f(components, nOutputs);
  std::vector<double> spanResult(nOutputs, std::numeric_limits<double>::quiet_NaN());
  spanOp(f, point, sg, std::span<double>(spanResult));
  const DataVector vectorResult = vectorOp(f, point, sg);
  BOOST_REQUIRE_EQUAL(vectorResult.size(), nOutputs);

  for (size_t k = 0; k < nOutputs; k++) {
    const double scalar = scalarOp(scalarFunc(k), point, sg);
    const double tol = 4.0 * ULP_TOL * EPS * scales[k] + 1e-300;
    BOOST_CHECK_SMALL(spanResult[k] - scalar, tol);
    BOOST_CHECK_SMALL(vectorResult[k] - scalar, tol);
  }
}

/// An external interpolation method that overrides only the scalar overload (Lagrange form).
class LagrangeMethod : public InterpolationMethod {
 public:
  LagrangeMethod() : InterpolationMethod(42) {}

  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override {
    double result = 0.0;
    for (size_t j = 0; j < nodes.size(); j++) {
      result += values[j] * lagrangeBasis(nodes.data(), nodes.size(), j, pos);
    }
    return result;
  }
};

/// m values per node (node-major) of the 1D test data.
std::vector<double> nodeValues(const std::vector<double>& nodes, const size_t nOutputs) {
  std::vector<double> values(nodes.size() * nOutputs);
  DataVector x(1);
  for (size_t i = 0; i < nodes.size(); i++) {
    x[0] = nodes[i];
    components(x, std::span<double>(values.data() + i * nOutputs, nOutputs));
  }
  return values;
}

/// The values of output @p k of @p values (m per node, node-major).
std::vector<double> outputValues(const std::vector<double>& values, const size_t nOutputs,
                                 const size_t k) {
  std::vector<double> result(values.size() / nOutputs);
  for (size_t i = 0; i < result.size(); i++) result[i] = values[i * nOutputs + k];
  return result;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(vector_global_interpolation)

BOOST_AUTO_TEST_CASE(Outputs_Equal_Scalar_Interpolation) {
  forAllConfigs({"equi", "cc", "cheb1"}, [](const SparseGrid& sg, const DataVector& point,
                                            const size_t nOutputs) {
    const DataVector normPoint = tools::normalizeDataVector(point, sg.getGenInstr().getDomain());
    const std::vector<double> scales = checkGlobalTensorGrids(sg, normPoint, nOutputs);
    checkOperator(
        sg, point, nOutputs, scales,
        [](const SourceFunc& f, const DataVector& p, const SparseGrid& g) {
          return interpolate(f, p, g);
        },
        [](const VectorSourceFunc& f, const DataVector& p, const SparseGrid& g,
           std::span<double> out) { interpolate(f, p, g, out); },
        [](const VectorSourceFunc& f, const DataVector& p, const SparseGrid& g) {
          return interpolate(f, p, g);
        });
  });
}

BOOST_AUTO_TEST_CASE(Wrong_Result_Size_Is_Rejected) {
  CompleteSGGenInstr genInstr(2, 2);
  const SparseGrid sg(genInstr);
  const VectorSourceFunc f(components, 3);
  const DataVector point{0.3, 0.6};
  std::vector<double> tooSmall(2), tooLarge(4);
  BOOST_CHECK_THROW(interpolate(f, point, sg, std::span<double>(tooSmall)), std::invalid_argument);
  BOOST_CHECK_THROW(interpolate(f, point, sg, std::span<double>(tooLarge)), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vector_linear_interpolation)

BOOST_AUTO_TEST_CASE(Outputs_Equal_Scalar_Interpolation) {
  forAllConfigs({"equi", "cc", "cheb1"}, [](const SparseGrid& sg, const DataVector& point,
                                            const size_t nOutputs) {
    const DataVector normPoint = tools::normalizeDataVector(point, sg.getGenInstr().getDomain());
    const std::vector<double> scales = checkLinearTensorGrids(sg, normPoint, nOutputs);
    checkOperator(
        sg, point, nOutputs, scales,
        [](const SourceFunc& f, const DataVector& p, const SparseGrid& g) {
          return interpolateLinear(f, p, g);
        },
        [](const VectorSourceFunc& f, const DataVector& p, const SparseGrid& g,
           std::span<double> out) { interpolateLinear(f, p, g, out); },
        [](const VectorSourceFunc& f, const DataVector& p, const SparseGrid& g) {
          return interpolateLinear(f, p, g);
        });
  });
}

BOOST_AUTO_TEST_CASE(Wrong_Result_Size_Is_Rejected) {
  CompleteSGGenInstr genInstr(2, 2);
  const SparseGrid sg(genInstr);
  const VectorSourceFunc f(components, 3);
  const DataVector point{0.3, 0.6};
  std::vector<double> tooSmall(2), tooLarge(4);
  BOOST_CHECK_THROW(interpolateLinear(f, point, sg, std::span<double>(tooSmall)),
                    std::invalid_argument);
  BOOST_CHECK_THROW(interpolateLinear(f, point, sg, std::span<double>(tooLarge)),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(multi_output_interpolation_method)

BOOST_AUTO_TEST_CASE(Default_Applies_Scalar_Formula_Per_Output) {
  const LagrangeMethod method;
  const InterpolationMethod& base = method;

  for (const GPCntType nNodes : {1u, 2u, 3u, 5u, 9u}) {
    const DataVector nodesVec = getClenshawCurtisNodeGenFunc()->genNodes(nNodes, nNodes > 1);
    const std::vector<double> nodes(nodesVec.begin(), nodesVec.end());
    for (const size_t nOutputs : {1, 3, 4}) {
      const std::vector<double> values = nodeValues(nodes, nOutputs);
      std::vector<double> out(nOutputs, std::numeric_limits<double>::quiet_NaN());
      base.interpolate(0.3141, nodes, values, nOutputs, out.data());
      for (size_t k = 0; k < nOutputs; k++) {
        BOOST_CHECK_EQUAL(out[k],
                          method.interpolate(0.3141, nodes, outputValues(values, nOutputs, k)));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Built_In_Overrides_Equal_Default) {
  const std::vector<std::pair<NodeGenFunc*, InterpolationMethod*>> methods = {
      {getClenshawCurtisNodeGenFunc(), getOptBarycentricFormula()},
      {getEquidistantNodeGenFunc(), getBarycentricFormula(getEquidistantNodeGenFunc())},
      {getFirstTypeChebyshevNodeGenFunc(),
       getBarycentricFormula(getFirstTypeChebyshevNodeGenFunc())}};

  for (const auto& [nodeGenFunc, method] : methods) {
    for (const GPCntType nNodes : {1u, 2u, 3u, 5u, 9u, 17u}) {
      const bool boundary = nodeGenFunc != getFirstTypeChebyshevNodeGenFunc() && nNodes > 1;
      const DataVector nodesVec = nodeGenFunc->genNodes(nNodes, boundary);
      const std::vector<double> nodes(nodesVec.begin(), nodesVec.end());
      for (const double pos : {0.3141, 0.5, nodes.back()}) {
        for (const size_t nOutputs : {1, 3, 4}) {
          BOOST_TEST_CONTEXT("method " << method->id() << " n" << nNodes << " pos " << pos << " m"
                                       << nOutputs) {
            const std::vector<double> values = nodeValues(nodes, nOutputs);
            std::vector<double> out(nOutputs), expected(nOutputs);
            method->interpolate(pos, nodes, values, nOutputs, out.data());
            method->InterpolationMethod::interpolate(pos, nodes, values, nOutputs, expected.data());
            for (size_t k = 0; k < nOutputs; k++) {
              double scale = 0.0;
              for (size_t i = 0; i < nodes.size(); i++) {
                scale += std::abs(lagrangeBasis(nodes.data(), nodes.size(), i, pos) *
                                  values[i * nOutputs + k]);
              }
              BOOST_CHECK_SMALL(out[k] - expected[k], ULP_TOL * EPS * scale + 1e-300);
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
