// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <omp.h>
#include <cmath>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/caching/source_function_caching/source_function_cache.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <utility>

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
  return SourceFunc([a, b](const DataVector& point) { return a * point[0] + b; });
}

SourceFunc genQuadraticSourceFunction(const double a, const double b, const double c) {
  return SourceFunc(
      ([a, b, c](const DataVector& point) { return a * point[0] * point[0] + b * point[1] + c; }));
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

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(0);

    const SparseGrid sg(genInstr);

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, 0, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 1;
    const double value = randGen.getUniformRN(-1, 1);

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = value;

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));  // 4...6
    const double value = randGen.getUniformRN(-1, 1);

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(value);

    const SparseGrid sg(genInstr);

    const double expected = nDim > 0 ? value : 0;

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(constant_func_random_cube_nD) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);  // 1..5
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));
    const double value = randGen.getUniformRN(-2.0, 2.0);

    FullSGGenInstr genInstr(maxLvl, nDim);

    double volume = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-2.0, 0.0);
      double b = randGen.getUniformRN(0.5, 3.0);
      genInstr.setBoundsForDim({a, b}, d);
      volume *= (b - a);
    }

    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    const double expected = value * volume;
    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8...10

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = -std::cos(1) + 1;

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_interval_1D) {
  randGen.setSeed(1772646822);
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = static_cast<LvlType>(9);

    const double a = randGen.getUniformRN(-1.0, 0.0);
    const double b = randGen.getUniformRN(0.5, 2.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setBoundsForDim({a, b}, 0);

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    // ∫ sin(x) dx = cos(a) - cos(b)
    const double expected = std::cos(a) - std::cos(b);
    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_2D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8...10

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double expected = 4 * std::pow(std::sin(0.5), 4);

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_3D) {
  // f(x,y,z) = sin(x)*sin(y)*sin(z) => integral = (1 - cos(1))^3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(8 + randGen.getUniformIndexRN(3));  // 8..10

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    const SparseGrid sg(genInstr);

    const double oneD = -std::cos(1.0) + 1.0;  // = 1 - cos(1)
    const double expected = oneD * oneD * oneD;

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
  }
}

BOOST_AUTO_TEST_CASE(sin_func_random_cube_3D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 3;
    const LvlType maxLvl = static_cast<LvlType>(9);

    FullSGGenInstr genInstr(maxLvl, nDim);

    double expected = 1.0;
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      genInstr.setBoundsForDim({a, b}, d);

      // ∫ sin(x) dx = cos(a) - cos(b)
      expected *= (std::cos(a) - std::cos(b));
    }

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    const double result = quadrature(sg, sourceFunc);

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

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genLinearSourceFunction(a, b);

    const SparseGrid sg(genInstr);

    const double expected = a * 0.5 + b;
    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(linear_func_random_interval_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 5;

    const double aCoeff = randGen.getUniformRN(-2.0, 2.0);
    const double bCoeff = randGen.getUniformRN(-1.0, 1.0);

    const double a = randGen.getUniformRN(-3.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 3.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setBoundsForDim({a, b}, 0);

    const SourceFunc sourceFunc = genLinearSourceFunction(aCoeff, bCoeff);
    const SparseGrid sg(genInstr);

    // ∫(a*x + b) dx = a/2 (b^2 - a^2) + b (b - a)
    const double expected = aCoeff * 0.5 * (b * b - a * a) + bCoeff * (b - a);

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(quadratic_func_unit_cube_1D) {
  // f(x) = x^2 on [0,1] => integral = 1/3
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 10;

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    const double expected = 1.0 / 3.0;
    const double result = quadrature(sg, sourceFunc);

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

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setBoundsForDim({a, b}, 0);

    const SourceFunc sourceFunc = genQuadraticSourceFunction(1, 0, 0);

    const SparseGrid sg(genInstr);

    // ∫ x^2 dx = (b^3 - a^3)/3
    const double expected = (b * b * b - a * a * a) / 3.0;
    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(monomial_unit_cube_random_nD) {
  // f(x) = \prod_i x_i^{k_i}  => Integral = \prod_i 1/(k_i+1)
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(4);                           // 1..4
    const LvlType maxLvl = static_cast<LvlType>(9 + randGen.getUniformIndexRN(3));  // 9..11

    // Random exponents 0..4
    std::vector<int> exponents(nDim);
    for (size_t i = 0; i < nDim; ++i) {
      exponents[i] = static_cast<int>(randGen.getUniformIndexRN(5));  // 0..4
    }

    const FullSGGenInstr genInstr(maxLvl, nDim);
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

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-10);
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

    FullSGGenInstr genInstr(maxLvl, nDim);

    for (size_t d = 0; d < nDim; ++d) {
      exponents[d] = static_cast<int>(randGen.getUniformIndexRN(4));  // 0..3
      double a = randGen.getUniformRN(-1.0, 0.0);
      double b = randGen.getUniformRN(0.5, 2.0);
      bounds[d] = {a, b};
      genInstr.setBoundsForDim(bounds[d], d);

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
    const double result = quadrature(sg, sourceFunc);

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

    const FullSGGenInstr genInstr(maxLvl, nDim);
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

    const double result = quadrature(sg, sourceFunc);

    BOOST_CHECK_CLOSE(result, expected, 1e-9);
  }
}

BOOST_AUTO_TEST_SUITE_END()