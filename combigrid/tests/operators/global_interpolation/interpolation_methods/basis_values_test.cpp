// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file basis_values_test.cpp
 * @brief Boost.Test cases for @c InterpolationMethod::basisValues: the weights of the values in
 * the 1D interpolant (default: one interpolation per node; overrides of the built-in barycentric
 * formulas: one pass).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cmath>
#include <cstddef>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;

namespace {

/// A method that only provides the scalar interpolant (linear in the values): inherits the
/// default basisValues.
class LinearCombinationMethod : public InterpolationMethod {
 public:
  LinearCombinationMethod() : InterpolationMethod(12345) {}

  double interpolate(const double pos, const std::vector<double>& nodes,
                     const std::vector<double>& values) const override {
    double sum = 0.0;
    for (size_t i = 0; i < nodes.size(); i++) sum += std::cos(pos * (1.0 + nodes[i])) * values[i];
    return sum;
  }
  using InterpolationMethod::interpolate;
};

std::vector<double> nodesOf(const NodeGenFunc& nodeGenFunc, const GPCntType nNodes,
                            const bool boundary) {
  const sgpp::base::DataVector nodes = nodeGenFunc.genNodes(nNodes, boundary);
  return std::vector<double>(nodes.begin(), nodes.end());
}

/// Values of a smooth function at the nodes.
std::vector<double> valuesAt(const std::vector<double>& nodes) {
  std::vector<double> values(nodes.size());
  for (size_t i = 0; i < nodes.size(); i++) values[i] = std::exp(nodes[i]) * std::sin(3 * nodes[i]);
  return values;
}

/// Checks sum_i out[i] * values[i] == interpolate within round-off (scaled by sum_i |out[i]|) and
/// the override against the default of the base class.
void checkMethod(const InterpolationMethod& method, const std::vector<double>& nodes,
                 const double pos, const std::string& name) {
  const std::vector<double> values = valuesAt(nodes);
  std::vector<double> weights(nodes.size()), defaultWeights(nodes.size());
  method.basisValues(pos, nodes, weights);
  method.InterpolationMethod::basisValues(pos, nodes, defaultWeights);

  double sum = 0.0, absSum = 0.0;
  for (size_t i = 0; i < nodes.size(); i++) {
    sum += weights[i] * values[i];
    absSum += std::abs(weights[i]);
  }
  const double expected = method.interpolate(pos, nodes, values);

  BOOST_TEST_CONTEXT(name << " n " << nodes.size() << " pos " << pos) {
    BOOST_CHECK_SMALL(sum - expected, 1e-14 * 8 * (1.0 + absSum));
    for (size_t i = 0; i < nodes.size(); i++) {
      BOOST_CHECK_SMALL(weights[i] - defaultWeights[i], 1e-14 * 8 * (1.0 + absSum));
    }
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(basis_values)

BOOST_AUTO_TEST_CASE(Built_In_Formulas_Equal_Interpolant_And_Default) {
  const std::vector<double> positions = {0.0, 0.137, 0.5, 0.7071, 1.0, -0.2, 1.3};
  for (const GPCntType nNodes : {1u, 2u, 3u, 5u, 9u, 17u, 33u}) {
    for (const double pos : positions) {
      // Generic formula: Chebyshev nodes of the first type, equidistant nodes with boundary;
      // optimized formula: Clenshaw-Curtis nodes with boundary
      NodeGenFunc* const cheb1 = getFirstTypeChebyshevNodeGenFunc();
      NodeGenFunc* const equi = getEquidistantNodeGenFunc();
      NodeGenFunc* const cc = getClenshawCurtisNodeGenFunc();
      checkMethod(*cheb1->getInterpolationMethod(nNodes), nodesOf(*cheb1, nNodes, false), pos,
                  "generic cheb1");
      checkMethod(*equi->getInterpolationMethod(nNodes), nodesOf(*equi, nNodes, nNodes >= 2), pos,
                  "generic equi");
      checkMethod(*cc->getInterpolationMethod(nNodes), nodesOf(*cc, nNodes, nNodes >= 2), pos,
                  "optimized cc");
    }
  }
}

BOOST_AUTO_TEST_CASE(Default_Probes_The_Scalar_Interpolant) {
  const LinearCombinationMethod method;
  const std::vector<double> nodes = {0.1, 0.4, 0.45, 0.9};
  std::vector<double> weights(nodes.size());
  method.basisValues(0.3, nodes, weights);
  for (size_t i = 0; i < nodes.size(); i++) {
    BOOST_CHECK_EQUAL(weights[i], std::cos(0.3 * (1.0 + nodes[i])));
  }
}

BOOST_AUTO_TEST_CASE(Trivial_Cases) {
  NodeGenFunc* const cc = getClenshawCurtisNodeGenFunc();
  const InterpolationMethod& method = *cc->getInterpolationMethod(5);

  std::vector<double> one(1);
  method.basisValues(0.7, {0.5}, one);
  BOOST_CHECK_EQUAL(one[0], 1.0);

  std::vector<double> two(2);
  method.basisValues(0.25, {0.0, 1.0}, two);
  BOOST_CHECK_EQUAL(two[0], 0.75);
  BOOST_CHECK_EQUAL(two[1], 0.25);

  // On a node (within the tolerance of interpolate): the unit vector of that node
  const std::vector<double> nodes = nodesOf(*cc, 5, true);
  std::vector<double> weights(5);
  method.basisValues(nodes[3] + 1e-15, nodes, weights);
  for (size_t i = 0; i < 5; i++) BOOST_CHECK_EQUAL(weights[i], (i == 3) ? 1.0 : 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
