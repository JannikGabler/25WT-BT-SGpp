// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file quadrature_test.cpp
 * @brief Boost.Test cases for the sparse-grid quadrature operator.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <algorithm>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

/*
f(x) = value
*/
SourceFunc genContantSourceFunction(const double value) {
  return SourceFunc([value](const DataVector& point) { return value; });
};

/*
f(x) = a * x + b
*/
SourceFunc genLinearSourceFunction(const double a, const double b) {
  return SourceFunc([a, b](const DataVector& point) {
    assert(point.size() == 1);
    return a * point[0] + b;
  });
}

SourceFunc genQuadraticSourceFunction(const double a, const double b, const double c) {
  return SourceFunc(([a, b, c](const DataVector& point) {
    assert(point.size() == 1);
    return a * point[0] * point[0] + b * point[0] + c;
  }));
}

/*
f(x) = \prod_{i=1}^d \sin(x_i)
*/
SourceFunc genSinSourceFunction() {
  return SourceFunc([](const DataVector& point) {
    double result = 1;

    for (const double v : point) {
      result *= std::sin(v);
    }

    return result;
  });
}

}  // namespace

BOOST_AUTO_TEST_SUITE(equidistant_nodes)

BOOST_AUTO_TEST_CASE(zero_func_unit_cube) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));  // 4...6

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(0);

    const SparseGrid sg(genInstr);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, 0, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 1;
    const double value = randGen.getUniformRN(-1, 1);

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = value;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);
    const LvlType maxLvl = static_cast<LvlType>(5 + randGen.getUniformIndexRN(3));  // 5...7
    const double value = randGen.getUniformRN(-1, 1);

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = nDim > 0 ? value : 0;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-08);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_random_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);  // 1..5
    const LvlType maxLvl = static_cast<LvlType>(7 + randGen.getUniformIndexRN(3));
    const double value = randGen.getUniformRN(-2.0, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);

    double volume = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-2.0, 0.0);
      double b = randGen.getUniformRN(0.5, 3.0);
      genInstr.setDomainForDim({a, b}, d);
      volume *= (b - a);
    }

    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    const double expected = value * volume;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-08);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8...10

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = -std::cos(1) + 1;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(9);

    const double a = randGen.getUniformRN(-1.0, 0.0);
    const double b = randGen.getUniformRN(0.5, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    // ∫ sin(x) dx = cos(a) - cos(b)
    const double expected = std::cos(a) - std::cos(b);
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_2D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8...10

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = 4 * std::pow(std::sin(0.5), 4);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_3D) {
  // f(x,y,z) = sin(x)*sin(y)*sin(z) => integral = (1 - cos(1))^3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8..10

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double oneD = -std::cos(1.0) + 1.0;  // = 1 - cos(1)
    const double expected = oneD * oneD * oneD;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_cube_3D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(9);

    CompleteSGGenInstr genInstr(maxLvl, nDim);

    double expected = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      genInstr.setDomainForDim({a, b}, d);

      // ∫ sin(x) dx = cos(a) - cos(b)
      expected *= (std::cos(a) - std::cos(b));
    }

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(linear_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 4;
    const double a = randGen.getUniformRN(-2.0, 2.0);
    const double b = randGen.getUniformRN(-1.0, 1.0);

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genLinearSourceFunction(a, b);

    const SparseGrid sg(genInstr);

    const double expected = a * 0.5 + b;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(linear_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 6;

    const double aCoeff = randGen.getUniformRN(-2.0, 2.0);
    const double bCoeff = randGen.getUniformRN(-1.0, 1.0);

    const double a = randGen.getUniformRN(-3.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 3.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genLinearSourceFunction(aCoeff, bCoeff);
    const SparseGrid sg(genInstr);

    // ∫(a*x + b) dx = a/2 (b^2 - a^2) + b (b - a)
    const double expected = aCoeff * 0.5 * (b * b - a * a) + bCoeff * (b - a);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(quadratic_func_unit_cube_1D) {
  // f(x) = x^2 on [0,1] => integral = 1/3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 10;

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    const double expected = 1.0 / 3.0;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(quadratic_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 10;

    const double a = randGen.getUniformRN(-2.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    // ∫ x^2 dx = (b^3 - a^3)/3
    const double expected = (b * b * b - a * a * a) / 3.0;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(monomial_unit_cube_random_nD) {
  // f(x) = \prod_i x_i^{k_i}  => Integral = \prod_i 1/(k_i+1)
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(3);                           // 1..3
    const LvlType maxLvl = static_cast<LvlType>(9 + randGen.getUniformIndexRN(3));  // 9..11

    // Random exponents 0..4
    std::vector<int> exponents(nDim);
    for (size_t i = 0; i < nDim; ++i) {
      exponents[i] = static_cast<int>(randGen.getUniformIndexRN(5));  // 0..4
    }

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc([exponents](const DataVector& point) {
      double v = 1.0;
      for (size_t i = 0; i < point.getSize(); ++i) {
        double xi = point[i];
        int k = exponents[i];
        double xi_pow = 1.0;
        for (int e = 0; e < k; ++e) xi_pow *= xi;
        v *= xi_pow;
      }
      return v;
    });

    const SparseGrid sg(genInstr);

    // expected = product_i 1/(k_i+1)
    double expected = 1.0;
    for (int k : exponents) {
      expected *= 1.0 / static_cast<double>(k + 1);
    }

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(monomial_random_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);  // 1..4
    const LvlType maxLvl = static_cast<LvlType>(8);

    std::vector<int> exponents(nDim);
    std::vector<std::pair<double, double>> bounds(nDim);

    double expected = 1.0;

    CompleteSGGenInstr genInstr(maxLvl, nDim);

    for (size_t d = 0; d < nDim; ++d) {
      exponents[d] = static_cast<int>(randGen.getUniformIndexRN(4));  // 0..3
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      bounds[d] = {a, b};
      genInstr.setDomainForDim(bounds[d], d);

      // ∫ x^k dx = (b^{k+1} - a^{k+1})/(k+1)
      expected *= (std::pow(b, exponents[d] + 1) - std::pow(a, exponents[d] + 1)) /
                  static_cast<double>(exponents[d] + 1);
    }

    const SourceFunc sourceFunc([exponents](const DataVector& point) {
      double v = 1.0;
      for (size_t i = 0; i < point.getSize(); ++i) {
        v *= std::pow(point[i], exponents[i]);
      }
      return v;
    });

    const SparseGrid sg(genInstr);
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-9);
  }
}

BOOST_AUTO_TEST_CASE(random_polynomial_unit_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);                           // 1..4
    const LvlType maxLvl = static_cast<LvlType>(6 + randGen.getUniformIndexRN(3));  // 6..8
    const size_t nTerms = 1 + randGen.getUniformIndexRN(4);                         // 1..4 Terme

    // Erzeuge Terme: jeder Term hat Koeff und Exponenten (0..3)
    struct Term {
      double coeff;
      std::vector<int> exps;
    };
    std::vector<Term> terms;
    for (size_t t = 0; t < nTerms; ++t) {
      Term term;
      term.coeff = randGen.getUniformRN(-2.0, 2.0);
      term.exps.resize(nDim);
      for (size_t i = 0; i < nDim; ++i) {
        term.exps[i] = static_cast<int>(randGen.getUniformIndexRN(4));  // 0..3
      }
      terms.push_back(term);
    }

    const CompleteSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc([terms](const DataVector& point) {
      double v = 0.0;
      for (const Term& term : terms) {
        double m = term.coeff;
        for (size_t i = 0; i < point.getSize(); ++i) {
          double xi = point[i];
          int k = term.exps[i];
          double xi_pow = 1.0;
          for (int e = 0; e < k; ++e) xi_pow *= xi;
          m *= xi_pow;
        }
        v += m;
      }
      return v;
    });

    const SparseGrid sg(genInstr);

    // Exact integral: Sum_{terms} coeff * prod_i 1/(k_i+1)
    double expected = 0.0;
    for (const Term& term : terms) {
      double contrib = term.coeff;
      for (int k : term.exps) {
        contrib *= 1.0 / static_cast<double>(k + 1);
      }
      expected += contrib;
    }

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-9);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(clenshaw_curtis_nodes)

BOOST_AUTO_TEST_CASE(zero_func_unit_cube) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));  // 4...6

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genContantSourceFunction(0);

    const SparseGrid sg(genInstr);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, 0, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 1;
    const double value = randGen.getUniformRN(-1, 1);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = value;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));  // 4...6
    const double value = randGen.getUniformRN(-1, 1);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = nDim > 0 ? value : 0;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_random_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);  // 1..5
    const LvlType maxLvl = static_cast<LvlType>(6 + randGen.getUniformIndexRN(3));
    const double value = randGen.getUniformRN(-2.0, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

    double volume = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-2.0, 0.0);
      double b = randGen.getUniformRN(0.5, 3.0);
      genInstr.setDomainForDim({a, b}, d);
      volume *= (b - a);
    }

    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    const double expected = value * volume;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-08);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(10 + randGen.getUniformIndexRN(3));  // 10...12

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = -std::cos(1) + 1;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(9);

    const double a = randGen.getUniformRN(-1.0, 0.0);
    const double b = randGen.getUniformRN(0.5, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    // ∫ sin(x) dx = cos(a) - cos(b)
    const double expected = std::cos(a) - std::cos(b);
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_2D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const LvlType maxLvl = static_cast<LvlType>(10 + randGen.getUniformIndexRN(3));  // 10...12

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = 4 * std::pow(std::sin(0.5), 4);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_3D) {
  // f(x,y,z) = sin(x)*sin(y)*sin(z) => integral = (1 - cos(1))^3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(10 + randGen.getUniformIndexRN(3));  // 10..12

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double oneD = -std::cos(1.0) + 1.0;  // = 1 - cos(1)
    const double expected = oneD * oneD * oneD;

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_cube_3D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(12);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

    double expected = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      genInstr.setDomainForDim({a, b}, d);

      // ∫ sin(x) dx = cos(a) - cos(b)
      expected *= (std::cos(a) - std::cos(b));
    }

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(linear_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 5;
    const double a = randGen.getUniformRN(-2.0, 2.0);
    const double b = randGen.getUniformRN(-1.0, 1.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genLinearSourceFunction(a, b);

    const SparseGrid sg(genInstr);

    const double expected = a * 0.5 + b;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(linear_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 6;

    const double aCoeff = randGen.getUniformRN(-2.0, 2.0);
    const double bCoeff = randGen.getUniformRN(-1.0, 1.0);

    const double a = randGen.getUniformRN(-3.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 3.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genLinearSourceFunction(aCoeff, bCoeff);
    const SparseGrid sg(genInstr);

    // ∫(a*x + b) dx = a/2 (b^2 - a^2) + b (b - a)
    const double expected = aCoeff * 0.5 * (b * b - a * a) + bCoeff * (b - a);

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(quadratic_func_unit_cube_1D) {
  // f(x) = x^2 on [0,1] => integral = 1/3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 12;

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    const double expected = 1.0 / 3.0;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(quadratic_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 10;

    const double a = randGen.getUniformRN(-2.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 2.0);

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    genInstr.setDomainForDim({a, b}, 0);

    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    // ∫ x^2 dx = (b^3 - a^3)/3
    const double expected = (b * b * b - a * a * a) / 3.0;
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(monomial_unit_cube_random_nD) {
  // f(x) = \prod_i x_i^{k_i}  => Integral = \prod_i 1/(k_i+1)
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(3);                           // 1..3
    const LvlType maxLvl = static_cast<LvlType>(9 + randGen.getUniformIndexRN(3));  // 9..11

    // Random exponents 0..4
    std::vector<int> exponents(nDim);
    for (size_t i = 0; i < nDim; ++i) {
      exponents[i] = static_cast<int>(randGen.getUniformIndexRN(5));  // 0..4
    }

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc([exponents](const DataVector& point) {
      double v = 1.0;
      for (size_t i = 0; i < point.getSize(); ++i) {
        double xi = point[i];
        int k = exponents[i];
        double xi_pow = 1.0;
        for (int e = 0; e < k; ++e) xi_pow *= xi;
        v *= xi_pow;
      }
      return v;
    });

    const SparseGrid sg(genInstr);

    // expected = product_i 1/(k_i+1)
    double expected = 1.0;
    for (int k : exponents) {
      expected *= 1.0 / static_cast<double>(k + 1);
    }

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-8);
  }
}

BOOST_AUTO_TEST_CASE(monomial_random_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);  // 1..4
    const LvlType maxLvl = static_cast<LvlType>(8);

    std::vector<int> exponents(nDim);
    std::vector<std::pair<double, double>> bounds(nDim);

    double expected = 1.0;

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

    for (size_t d = 0; d < nDim; ++d) {
      exponents[d] = static_cast<int>(randGen.getUniformIndexRN(4));  // 0..3
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      bounds[d] = {a, b};
      genInstr.setDomainForDim(bounds[d], d);

      // ∫ x^k dx = (b^{k+1} - a^{k+1})/(k+1)
      expected *= (std::pow(b, exponents[d] + 1) - std::pow(a, exponents[d] + 1)) /
                  static_cast<double>(exponents[d] + 1);
    }

    const SourceFunc sourceFunc([exponents](const DataVector& point) {
      double v = 1.0;
      for (size_t i = 0; i < point.getSize(); ++i) {
        v *= std::pow(point[i], exponents[i]);
      }
      return v;
    });

    const SparseGrid sg(genInstr);
    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-9);
  }
}

BOOST_AUTO_TEST_CASE(random_polynomial_unit_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);                           // 1..4
    const LvlType maxLvl = static_cast<LvlType>(6 + randGen.getUniformIndexRN(3));  // 6..8
    const size_t nTerms = 1 + randGen.getUniformIndexRN(4);                         // 1..4 Terme

    // Erzeuge Terme: jeder Term hat Koeff und Exponenten (0..3)
    struct Term {
      double coeff;
      std::vector<int> exps;
    };
    std::vector<Term> terms;
    for (size_t t = 0; t < nTerms; ++t) {
      Term term;
      term.coeff = randGen.getUniformRN(-2.0, 2.0);
      term.exps.resize(nDim);
      for (size_t i = 0; i < nDim; ++i) {
        term.exps[i] = static_cast<int>(randGen.getUniformIndexRN(4));  // 0..3
      }
      terms.push_back(term);
    }

    CompleteSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc([terms](const DataVector& point) {
      double v = 0.0;
      for (const Term& term : terms) {
        double m = term.coeff;
        for (size_t i = 0; i < point.getSize(); ++i) {
          double xi = point[i];
          int k = term.exps[i];
          double xi_pow = 1.0;
          for (int e = 0; e < k; ++e) xi_pow *= xi;
          m *= xi_pow;
        }
        v += m;
      }
      return v;
    });

    const SparseGrid sg(genInstr);

    // Exact integral: Sum_{terms} coeff * prod_i 1/(k_i+1)
    double expected = 0.0;
    for (const Term& term : terms) {
      double contrib = term.coeff;
      for (int k : term.exps) {
        contrib *= 1.0 / static_cast<double>(k + 1);
      }
      expected += contrib;
    }

    const double result = quadrature(sourceFunc, sg);

    BOOST_CHECK_CLOSE(result, expected, 1e-9);
  }
}

BOOST_AUTO_TEST_CASE(multiindex_vector_sg_gen_instr) {
  const SourceFunc sourceFunc = genSinSourceFunction();

  // nDim = 3
  const LvlMIVec mis{{5, 5, 0}, {0, 4, 1}, {5, 3, 2}, {0, 2, 3}, {0, 1, 4}, {0, 0, 5}};
  MIVecSGGenInstr genInstr(mis);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
  genInstr.setDomain({0, 2 * M_PI});

  const SparseGrid sg(genInstr);

  const double result = quadrature(sourceFunc, sg);

  constexpr double expected = 0.0;

  BOOST_REQUIRE(tools::nearlyEqual(result, expected, 1e-08, 1e-08));
}

BOOST_AUTO_TEST_SUITE_END()

/*******************************************************************************
Tensor grids of the quadrature operator, compared with a naive sum over the grid points
*******************************************************************************/

namespace {

NodeGenFunc* const equidistantNodeGenFunc = getEquidistantNodeGenFunc();
NodeGenFunc* const clenshawCurtisNodeGenFunc = getClenshawCurtisNodeGenFunc();

/// f(x) = exp(-|x - 0.3|^2) * (1 + x_0): not separable, distinguishes the grid points
double gaussianFunc(const DataVector& x) {
  double sqDist = 0.0;
  for (const double v : x) {
    sqDist += (v - 0.3) * (v - 0.3);
  }
  return std::exp(-sqDist) * (1.0 + x[0]);
}

/// Random domain with intervals inside [-2, 2] of length >= 0.5.
HyperCubeArea randomDomain(const size_t nDim) {
  HyperCubeArea domain(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    const double lower = randGen.getUniformRN(-2.0, 1.0);
    domain[dim] = {lower, lower + randGen.getUniformRN(0.5, 1.0)};
  }

  return domain;
}

/// Random multi-index with components in [0, @p maxComp].
LvlMI randomMI(const size_t nDim, const LvlType maxComp) {
  LvlMI mi(nDim);

  for (size_t dim = 0; dim < nDim; dim++) {
    mi[dim] = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));
  }

  return mi;
}

/// Result of the naive quadrature of one tensor grid.
struct NaiveTGQuadrature {
  double sum;     ///< Sum over the grid points of weight * f(grid point on the domain).
  double absSum;  ///< Sum of |weight * f(grid point on the domain)| (scale for tolerances).
  std::vector<std::vector<double>> points;  ///< The grid points on the domain.
};

/**
 * Naive tensor-product quadrature of @p tg on the unit domain (i.e. without the volume): for
 * every linear grid-point index, TensorGrid::getGridPointAndMI, the product of the 1D weights of
 * the quadrature rules of the node generation functions, and f at the point mapped onto the
 * domain.
 */
NaiveTGQuadrature naiveTGQuadrature(const SourceFunc& sourceFunc, const TensorGrid& tg,
                                    const SGGenInstr& genInstr) {
  const size_t nDim = tg.nDim();
  const GPMI& nGPPerDim = tg.getGPCntPerDim();
  const HyperCubeArea& domain = genInstr.getDomain();

  std::vector<DataVector> weights1D(nDim);
  for (size_t dim = 0; dim < nDim; dim++) {
    weights1D[dim].resize(nGPPerDim[dim]);
    genInstr.getNodeGenFuncForDim(dim)
        ->getQuadRule(nGPPerDim[dim])
        ->genWeightsInplace(weights1D[dim]);
  }

  NaiveTGQuadrature result{0.0, 0.0, {}};
  DataVector gp(nDim);
  GPMI gpMI(nDim);

  for (size_t idx = 0; idx < tg.nGP(); idx++) {
    tg.getGridPointAndMI(idx, gp, gpMI);

    double weight = 1.0;
    for (size_t dim = 0; dim < nDim; dim++) {
      weight *= weights1D[dim][gpMI[dim]];
      gp[dim] = gp[dim] * (domain[dim].second - domain[dim].first) + domain[dim].first;
    }

    const double summand = weight * sourceFunc.evaluate(gp);
    result.sum += summand;
    result.absSum += std::abs(summand);
    result.points.emplace_back(gp.begin(), gp.end());
  }

  return result;
}

/// Checks that @p actual and @p expected contain the same points (up to round-off), in any order.
void checkSamePoints(std::vector<std::vector<double>> actual,
                     std::vector<std::vector<double>> expected) {
  BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());

  for (size_t i = 0; i < actual.size(); i++) {
    BOOST_REQUIRE_EQUAL(actual[i].size(), expected[i].size());

    for (size_t dim = 0; dim < actual[i].size(); dim++) {
      BOOST_REQUIRE_MESSAGE(
          std::abs(actual[i][dim] - expected[i][dim]) <= 1e-12 * (1.0 + std::abs(expected[i][dim])),
          "point " << i << ", dim " << dim << ": " << actual[i][dim] << " vs. "
                   << expected[i][dim]);
    }
  }
}

/// Tolerance for sums of @p absSum magnitude that differ only in the order of the operations.
double roundOffTol(const double absSum) { return 1e-13 * std::max(1.0, absSum); }

/**
 * Compares processTensorGrid with the naive sum for every tensor grid of the sparse grid of
 * @p genInstr (evaluated points, number of evaluations, result) and quadrature() with the
 * naive combination.
 */
void checkAgainstNaiveSum(const SGGenInstr& genInstr) {
  const SparseGrid sg(genInstr);
  const size_t nDim = sg.nDim();

  std::vector<std::vector<double>> evaluatedPoints;
  const SourceFunc recordingFunc([&evaluatedPoints](const DataVector& x) {
    evaluatedPoints.emplace_back(x.begin(), x.end());
    return gaussianFunc(x);
  });
  const SourceFunc sourceFunc(gaussianFunc);

  const misc::QuadratureContext context{.sparseGrid = sg,
                                        .domain = sg.getGenInstr().getDomain(),
                                        .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
  misc::QuadratureScratch scratch(nDim, sg.getMaxTGSumOverGPCntsPerDim());

  double expectedTotal = 0.0;
  double absTotal = 0.0;

  for (const TensorGridCTData& tgData : sg) {
    const NaiveTGQuadrature expected = naiveTGQuadrature(sourceFunc, tgData.tensorGrid, genInstr);

    evaluatedPoints.clear();
    const double result =
        quadrature_operator::processTensorGrid(context, recordingFunc, tgData.tensorGrid, scratch);

    BOOST_CHECK_EQUAL(evaluatedPoints.size(), tgData.tensorGrid.nGP());
    checkSamePoints(evaluatedPoints, expected.points);
    BOOST_CHECK_MESSAGE(std::abs(result - expected.sum) <= roundOffTol(expected.absSum),
                        "processTensorGrid: " << result << ", naive: " << expected.sum
                                              << ", difference: " << result - expected.sum);

    expectedTotal += tgData.coefficient * expected.sum;
    absTotal += std::abs(tgData.coefficient) * expected.absSum;
  }

  const double volume = genInstr.getVolumeOfDomain();
  const double result = quadrature(sourceFunc, sg);
  BOOST_CHECK_MESSAGE(std::abs(result - volume * expectedTotal) <= volume * roundOffTol(absTotal),
                      "quadrature: " << result << ", naive: " << volume * expectedTotal
                                     << ", difference: " << result - volume * expectedTotal);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(quadrature_operator_tensor_grids)

BOOST_AUTO_TEST_CASE(Process_Tensor_Grid_Matches_Naive_Sum) {
  // Anisotropic tensor grids (complete and random MI sets), all node families with a quadrature
  // rule (also mixed), odd and even node counts (doubling and linear growth), boundary level
  // offsets 0, 1, 2 and unit as well as shifted and scaled domains, d = 1, ..., 4
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (const std::string nodes : {"equidistant", "Clenshaw-Curtis", "mixed"}) {
      for (const Lvl2GPCntFunc lvl2GPCntFunc :
           {&doublingLvl2GPCntFunction, &linearLvl2GPCntFunction}) {
        for (const LvlType boundaryLvlOffset : {0u, 1u, 2u}) {
          for (size_t nDim = 1; nDim <= 4; nDim++) {
            for (const bool unitDomain : {true, false}) {
              const size_t nInputMIs = 1 + randGen.getUniformIndexRN(3);
              std::vector<LvlMI> inputMIs;
              for (size_t i = 0; i < nInputMIs; i++) {
                inputMIs.push_back(randomMI(nDim, 3));
              }
              const LvlMIVec mis(inputMIs);

              CompleteSGGenInstr completeGenInstr(
                  static_cast<LvlType>(randGen.getUniformIndexRN(4)), nDim);
              MIVecSGGenInstr miVecGenInstr(mis);

              for (SGGenInstr* const genInstr : {static_cast<SGGenInstr*>(&completeGenInstr),
                                                 static_cast<SGGenInstr*>(&miVecGenInstr)}) {
                for (size_t dim = 0; dim < nDim; dim++) {
                  NodeGenFunc* const nodeGenFunc =
                      (nodes == "equidistant" ||
                       (nodes == "mixed" && randGen.getUniformIndexRN(2) == 0))
                          ? equidistantNodeGenFunc
                          : clenshawCurtisNodeGenFunc;
                  genInstr->setNodeGenFuncForDim(nodeGenFunc, dim);
                }
                genInstr->setLvl2GPCntFunc(lvl2GPCntFunc);
                genInstr->setBoundaryLevelOffset(boundaryLvlOffset);
                genInstr->setDomain(unitDomain ? HyperCubeArea(nDim, {0.0, 1.0})
                                               : randomDomain(nDim));

                BOOST_TEST_CONTEXT(
                    nodes << " nodes, "
                          << (lvl2GPCntFunc == &doublingLvl2GPCntFunction ? "doubling" : "linear")
                          << " growth, boundary level offset " << boundaryLvlOffset << ", nDim "
                          << nDim << (unitDomain ? ", unit domain" : ", random domain")
                          << (genInstr == &completeGenInstr ? ", complete" : ", MIVec")) {
                  checkAgainstNaiveSum(*genInstr);
                }
              }
            }
          }
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(Single_Point_Tensor_Grids) {
  // Boundary level offset 1: the tensor grid of level 0 has one grid point (the center), rows of
  // length 1 in dimension 0
  for (size_t nDim = 1; nDim <= 4; nDim++) {
    BOOST_TEST_CONTEXT("nDim " << nDim) {
      CompleteSGGenInstr genInstr(0, nDim);
      genInstr.setBoundaryLevelOffset(1);
      genInstr.setDomain(randomDomain(nDim));

      const SparseGrid sg(genInstr);
      BOOST_REQUIRE_EQUAL(sg.nTG(), 1u);
      BOOST_REQUIRE_EQUAL(sg.begin()->tensorGrid.nGP(), 1u);

      checkAgainstNaiveSum(genInstr);
    }
  }
}

BOOST_AUTO_TEST_CASE(Zero_Point_Tensor_Grid) {
  // A tensor grid without grid points contributes 0 and does not evaluate the source function
  CompleteSGGenInstr genInstr(2, 3);
  const SparseGrid sg(genInstr);

  size_t nEvaluations = 0;
  const SourceFunc sourceFunc([&nEvaluations](const DataVector&) {
    nEvaluations++;
    return 1.0;
  });
  const misc::QuadratureContext context{.sparseGrid = sg,
                                        .domain = sg.getGenInstr().getDomain(),
                                        .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
  misc::QuadratureScratch scratch(3, sg.getMaxTGSumOverGPCntsPerDim());

  for (const GPMI& nGPPerDim : {GPMI{3, 0, 5}, GPMI{0, 3, 5}, GPMI{3, 5, 0}}) {
    const TensorGrid tg(nGPPerDim);
    BOOST_REQUIRE_EQUAL(tg.nGP(), 0u);
    BOOST_CHECK_EQUAL(quadrature_operator::processTensorGrid(context, sourceFunc, tg, scratch),
                      0.0);
  }

  BOOST_CHECK_EQUAL(nEvaluations, 0u);
}

BOOST_AUTO_TEST_SUITE_END()
