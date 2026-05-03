// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file barycentric_formula_test.cpp
 * @brief Boost.Test cases for the generic barycentric Lagrange
 * interpolation method (with selectable node generator).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/interpolation_method.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

/****************
Equidistant nodes
****************/
BOOST_AUTO_TEST_SUITE(equidistant_nodes)

BOOST_AUTO_TEST_CASE(empty_case) {
  const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(0, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
  const std::vector<double> values(0);

  const double expected = 0;
  const double result = formula->interpolate(0.5, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(one_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(1, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1)};

    const double expected = values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(two_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(2, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1), randGen.getUniformRN(-1, 1)};

    const double expected =
        (values[1] - values[0]) / (nodes[1] - nodes[0]) * (point - nodes[0]) + values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(eval_at_node_case) {
  constexpr size_t nNodes = 5;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

    std::vector<double> values(nNodes);
    for (size_t i = 0; i < nNodes; i++) {
      values[i] = randGen.getUniformRN(-1, 1);
    }

    for (size_t i = 0; i < nNodes; i++) {
      const double result = formula->interpolate(nodes[i], nodes, values);
      BOOST_CHECK_CLOSE(result, values[i], 1e-12);
    }
  }
}

BOOST_AUTO_TEST_CASE(constant_function_case) {
  constexpr size_t nNodes = 6;
  constexpr double point = 0.37;

  const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  const double c = 3.141592653589793;
  const std::vector<double> values(nNodes, c);

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, c, 1e-12);
}

BOOST_AUTO_TEST_CASE(linear_polynomial_case) {
  constexpr size_t nNodes = 4;
  constexpr double point = 0.42;
  constexpr double a = 2.5;
  constexpr double b = -0.7;

  const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] + b;
  }

  const double expected = a * point + b;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(quadratic_polynomial_case) {
  constexpr size_t nNodes = 5;
  constexpr double point = 0.51;
  constexpr double a = 1.2;
  constexpr double b = -0.8;
  constexpr double c = 0.3;

  const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] * nodes[i] + b * nodes[i] + c;
  }

  const double expected = a * point * point + b * point + c;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_CASE(extrapolation_case) {
  constexpr size_t nNodes = 3;
  constexpr double point = 1.5;

  const NodeGenFunc* nodeGenFunc = getEquidistantNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = nodes[i] * nodes[i];
  }

  const double expected = point * point;
  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_SUITE_END()

/*************************
First type Chebyshev nodes
*************************/
BOOST_AUTO_TEST_SUITE(first_type_chebyshev_nodes)

BOOST_AUTO_TEST_CASE(empty_case) {
  const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(0, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
  const std::vector<double> values(0);

  const double expected = 0;
  const double result = formula->interpolate(0.5, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(one_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(1, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1)};

    const double expected = values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(two_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(2, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1), randGen.getUniformRN(-1, 1)};

    const double expected =
        (values[1] - values[0]) / (nodes[1] - nodes[0]) * (point - nodes[0]) + values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(eval_at_node_case) {
  constexpr size_t nNodes = 5;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

    std::vector<double> values(nNodes);
    for (size_t i = 0; i < nNodes; i++) {
      values[i] = randGen.getUniformRN(-1, 1);
    }

    for (size_t i = 0; i < nNodes; i++) {
      const double result = formula->interpolate(nodes[i], nodes, values);
      BOOST_CHECK_CLOSE(result, values[i], 1e-12);
    }
  }
}

BOOST_AUTO_TEST_CASE(constant_function_case) {
  constexpr size_t nNodes = 6;
  constexpr double point = 0.37;

  const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  const double c = 3.141592653589793;
  const std::vector<double> values(nNodes, c);

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, c, 1e-12);
}

BOOST_AUTO_TEST_CASE(linear_polynomial_case) {
  constexpr size_t nNodes = 4;
  constexpr double point = 0.42;
  constexpr double a = 2.5;
  constexpr double b = -0.7;

  const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] + b;
  }

  const double expected = a * point + b;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(quadratic_polynomial_case) {
  constexpr size_t nNodes = 5;
  constexpr double point = 0.51;
  constexpr double a = 1.2;
  constexpr double b = -0.8;
  constexpr double c = 0.3;

  const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] * nodes[i] + b * nodes[i] + c;
  }

  const double expected = a * point * point + b * point + c;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_CASE(extrapolation_case) {
  constexpr size_t nNodes = 3;
  constexpr double point = 1.5;

  const NodeGenFunc* nodeGenFunc = getFirstTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = nodes[i] * nodes[i];
  }

  const double expected = point * point;
  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_SUITE_END()

/**************************
Second type Chebyshev nodes
**************************/
BOOST_AUTO_TEST_SUITE(second_type_chebyshev_nodes)

BOOST_AUTO_TEST_CASE(empty_case) {
  const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(0, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
  const std::vector<double> values(0);

  const double expected = 0;
  const double result = formula->interpolate(0.5, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(one_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(1, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1)};

    const double expected = values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(two_value_case) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const double point = randGen.getUniformRN(0, 1);

    const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(2, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);
    const std::vector<double> values{randGen.getUniformRN(-1, 1), randGen.getUniformRN(-1, 1)};

    const double expected =
        (values[1] - values[0]) / (nodes[1] - nodes[0]) * (point - nodes[0]) + values[0];
    const double result = formula->interpolate(point, nodes, values);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(eval_at_node_case) {
  constexpr size_t nNodes = 5;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
    const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
    const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

    std::vector<double> values(nNodes);
    for (size_t i = 0; i < nNodes; i++) {
      values[i] = randGen.getUniformRN(-1, 1);
    }

    for (size_t i = 0; i < nNodes; i++) {
      const double result = formula->interpolate(nodes[i], nodes, values);
      BOOST_CHECK_CLOSE(result, values[i], 1e-12);
    }
  }
}

BOOST_AUTO_TEST_CASE(constant_function_case) {
  constexpr size_t nNodes = 6;
  constexpr double point = 0.37;

  const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  const double c = 3.141592653589793;
  const std::vector<double> values(nNodes, c);

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, c, 1e-12);
}

BOOST_AUTO_TEST_CASE(linear_polynomial_case) {
  constexpr size_t nNodes = 4;
  constexpr double point = 0.42;
  constexpr double a = 2.5;
  constexpr double b = -0.7;

  const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();

  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] + b;
  }

  const double expected = a * point + b;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-12);
}

BOOST_AUTO_TEST_CASE(quadratic_polynomial_case) {
  constexpr size_t nNodes = 5;
  constexpr double point = 0.51;
  constexpr double a = 1.2;
  constexpr double b = -0.8;
  constexpr double c = 0.3;

  const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = a * nodes[i] * nodes[i] + b * nodes[i] + c;
  }

  const double expected = a * point * point + b * point + c;

  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_CASE(extrapolation_case) {
  constexpr size_t nNodes = 3;
  constexpr double point = 1.5;

  const NodeGenFunc* nodeGenFunc = getSecondTypeChebyshevNodeGenFunc();
  const DataVector nodes = nodeGenFunc->genNodes(nNodes, false);
  const InterpolationMethod* formula = getBarycentricFormula(nodeGenFunc);

  std::vector<double> values(nNodes);
  for (size_t i = 0; i < nNodes; i++) {
    values[i] = nodes[i] * nodes[i];
  }

  const double expected = point * point;
  const double result = formula->interpolate(point, nodes, values);

  BOOST_CHECK_CLOSE(result, expected, 1e-11);
}

BOOST_AUTO_TEST_SUITE_END()