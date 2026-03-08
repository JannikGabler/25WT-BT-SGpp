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
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/interpolation/interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
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
  return SourceFunc([value](const DataVector& /*point*/) { return value; });
};

/*
 f(x) = a * x_0 + b  (only uses first coordinate)
*/
SourceFunc genLinearSourceFunction(const double a, const double b) {
  return SourceFunc([a, b](const DataVector& point) { return a * point[0] + b; });
}

/*
 f(x) = a * x0^2 + b * x1 + c (2D example)
*/
SourceFunc genQuadraticSourceFunction(const double a, const double b, const double c) {
  return SourceFunc(
      ([a, b, c](const DataVector& point) { return a * point[0] * point[0] + b * point[1] + c; }));
}

/*
 f(x) = prod_i sin(x_i)
*/
SourceFunc genSinSourceFunction() {
  return SourceFunc([](const DataVector& point) {
    double result = 1.0;
    for (size_t i = 0; i < point.getSize(); ++i) result *= std::sin(point[i]);
    return result;
  });
}

DataVector genRandomPointInBounds(const size_t dim,
                                  const std::vector<std::pair<double, double>>& bounds = {}) {
  DataVector pt(dim);
  for (size_t d = 0; d < dim; ++d) {
    double a = 0.0;
    double b = 1.0;
    if (!bounds.empty()) {
      a = bounds[d].first;
      b = bounds[d].second;
    }
    pt[d] = randGen.getUniformRN(a, b);
  }
  return pt;
}

}  // namespace

/****************
Equidistant nodes
****************/
BOOST_AUTO_TEST_SUITE(equidistant_node_interpolation)

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_nD_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);  // 0..4 dims possible -> treat 0 as trivial
    const LvlType maxLvl = static_cast<LvlType>(1 + randGen.getUniformIndexRN(3));  // 1...3
    const double value = randGen.getUniformRN(-3.0, 3.0);

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    // sample several random points (unit cube default)
    const size_t nSamples = (nDim == 0) ? 1 : 5;
    for (size_t s = 0; s < nSamples; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double result = interpolate(sourceFunc, pt, sg);
      const double expected = (nDim > 0) ? value : 0.0;
      BOOST_CHECK_CLOSE(result, expected, 1e-8);
    }
  }
}

BOOST_AUTO_TEST_CASE(constant_func_random_cube_nD_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);                           // 1..6 dims
    const LvlType maxLvl = static_cast<LvlType>(1 + randGen.getUniformIndexRN(3));  // 1..3
    const double value = randGen.getUniformRN(-2.0, 2.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    std::vector<std::pair<double, double>> bounds(nDim);
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-2.0, 0.0);
      double b = randGen.getUniformRN(0.5, 3.0);
      genInstr.setBoundsForDim({a, b}, d);
      bounds[d] = {a, b};
    }

    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    // check interpolation at several random points in the domain
    for (size_t s = 0; s < 10; ++s) {
      DataVector pt = genRandomPointInBounds(nDim, bounds);
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, value, 1e-10);
    }
  }
}

BOOST_AUTO_TEST_CASE(linear_func_unit_cube_1D_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 3;
    const double a = randGen.getUniformRN(-5.0, 5.0);
    const double b = randGen.getUniformRN(-2.0, 2.0);

    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genLinearSourceFunction(a, b);
    const SparseGrid sg(genInstr);

    // mehrere zufällige Testpunkte
    for (size_t s = 0; s < 20; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double expected = a * pt[0] + b;
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, expected, 1e-8);
    }
  }
}

BOOST_AUTO_TEST_CASE(linear_func_random_interval_1D_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 3;

    const double aCoeff = randGen.getUniformRN(-3.0, 3.0);
    const double bCoeff = randGen.getUniformRN(-1.0, 1.0);

    const double a = randGen.getUniformRN(-4.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 4.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setBoundsForDim({a, b}, 0);

    const SourceFunc sourceFunc = genLinearSourceFunction(aCoeff, bCoeff);
    const SparseGrid sg(genInstr);

    for (size_t s = 0; s < 20; ++s) {
      DataVector pt = genRandomPointInBounds(nDim, {{a, b}});
      const double expected = aCoeff * pt[0] + bCoeff;
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, expected, 1e-8);
    }
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_2D_interpolation_accuracy) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const LvlType maxLvl = 3;
    const FullSGGenInstr genInstr(maxLvl, nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    // Check multiple random points
    for (size_t s = 0; s < 12; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double expected = sourceFunc.evaluate(pt);
      const double result = interpolate(sourceFunc, pt, sg);

      BOOST_CHECK_CLOSE(result, expected, 1e-4);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

/********************
Clenshaw-Curtis nodes
********************/
BOOST_AUTO_TEST_SUITE(clenshaw_curtis_node_interpolation)

BOOST_AUTO_TEST_CASE(constant_func_unit_cube_nD_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = randGen.getUniformIndexRN(5);  // 0..4 dims possible -> treat 0 as trivial
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));  // 4...6
    const double value = randGen.getUniformRN(-3.0, 3.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    // sample several random points (unit cube default)
    const size_t nSamples = (nDim == 0) ? 1 : 5;
    for (size_t s = 0; s < nSamples; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double result = interpolate(sourceFunc, pt, sg);
      const double expected = (nDim > 0) ? value : 0.0;
      BOOST_CHECK_CLOSE(result, expected, 1e-8);
    }
  }
}

BOOST_AUTO_TEST_CASE(constant_func_random_cube_nD_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);  // 1..6 dims
    const LvlType maxLvl = static_cast<LvlType>(4 + randGen.getUniformIndexRN(3));
    const double value = randGen.getUniformRN(-2.0, 2.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    std::vector<std::pair<double, double>> bounds(nDim);
    for (size_t d = 0; d < nDim; ++d) {
      double a = randGen.getUniformRN(-2.0, 0.0);
      double b = randGen.getUniformRN(0.5, 3.0);
      genInstr.setBoundsForDim({a, b}, d);
      bounds[d] = {a, b};
    }

    const SourceFunc sourceFunc = genContantSourceFunction(value);
    const SparseGrid sg(genInstr);

    // check interpolation at several random points in the domain
    for (size_t s = 0; s < 10; ++s) {
      DataVector pt = genRandomPointInBounds(nDim, bounds);
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, value, 1e-10);
    }
  }
}

BOOST_AUTO_TEST_CASE(linear_func_unit_cube_1D_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 6;
    const double a = randGen.getUniformRN(-5.0, 5.0);
    const double b = randGen.getUniformRN(-2.0, 2.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    const SourceFunc sourceFunc = genLinearSourceFunction(a, b);
    const SparseGrid sg(genInstr);

    // mehrere zufällige Testpunkte
    for (size_t s = 0; s < 20; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double expected = a * pt[0] + b;
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, expected, 1e-12);
    }
  }
}

BOOST_AUTO_TEST_CASE(linear_func_random_interval_1D_interpolation) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 1;
    const LvlType maxLvl = 6;

    const double aCoeff = randGen.getUniformRN(-3.0, 3.0);
    const double bCoeff = randGen.getUniformRN(-1.0, 1.0);

    const double a = randGen.getUniformRN(-4.0, -0.5);
    const double b = randGen.getUniformRN(0.5, 4.0);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());
    genInstr.setBoundsForDim({a, b}, 0);

    const SourceFunc sourceFunc = genLinearSourceFunction(aCoeff, bCoeff);
    const SparseGrid sg(genInstr);

    for (size_t s = 0; s < 20; ++s) {
      DataVector pt = genRandomPointInBounds(nDim, {{a, b}});
      const double expected = aCoeff * pt[0] + bCoeff;
      const double result = interpolate(sourceFunc, pt, sg);
      BOOST_CHECK_CLOSE(result, expected, 1e-10);
    }
  }
}

BOOST_AUTO_TEST_CASE(sin_func_unit_cube_2D_interpolation_accuracy) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const LvlType maxLvl = static_cast<LvlType>(10);

    FullSGGenInstr genInstr(maxLvl, nDim);
    genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

    const SourceFunc sourceFunc = genSinSourceFunction();
    const SparseGrid sg(genInstr);

    // Check multiple random points
    for (size_t s = 0; s < 12; ++s) {
      DataVector pt = genRandomPointInBounds(nDim);
      const double expected = sourceFunc.evaluate(pt);
      const double result = interpolate(sourceFunc, pt, sg);

      BOOST_CHECK_CLOSE(result, expected, 1e-6);
    }
  }
}

/*
Convergence test: Error should decrease with increasing level.
*/
BOOST_AUTO_TEST_CASE(interpolation_converges_with_level_1D) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const size_t nDim = 2;
    const DataVector testPoint = genRandomPointInBounds(nDim);
    const SourceFunc sourceFunc = genSinSourceFunction();

    double prevError = std::numeric_limits<double>::infinity();

    for (const LvlType lvl : std::vector<LvlType>{1, 2, 3, 4, 5, 6, 7}) {
      FullSGGenInstr genInstr(lvl, nDim);
      genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

      const SparseGrid sg(genInstr);
      const double result = interpolate(sourceFunc, testPoint, sg);
      const double expected = sourceFunc.evaluate(testPoint);
      const double err = std::abs(result - expected);

      BOOST_REQUIRE_SMALL(err, 1.0);

      BOOST_CHECK_MESSAGE(
          err < 1e-15 || prevError / err >= 3.5,
          "The error '"
              << err << "' was not reduced by an expected amount compared to the previous error '"
              << prevError << "'.");
      prevError = err;
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()