// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file vector_source_function_test.cpp
 * @brief Boost.Test cases for @c VectorSourceFunc.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <functional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <vector>

using sgpp::base::DataVector;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::VectorSourceFunc;

namespace {

/// Output k: sum_i (i + 1) * x_i + k.
void weightedSums(const DataVector& x, std::span<double> out) {
  for (size_t k = 0; k < out.size(); k++) {
    double sum = static_cast<double>(k);
    for (size_t i = 0; i < x.size(); i++) sum += static_cast<double>(i + 1) * x[i];
    out[k] = sum;
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(vector_source_function)

BOOST_AUTO_TEST_CASE(evaluate_forwards_to_the_callable) {
  const std::function<void(const DataVector&, std::span<double>)> callable(weightedSums);
  const VectorSourceFunc byCopy(callable, 3);
  const VectorSourceFunc byMove(
      [](const DataVector& x, std::span<double> out) { weightedSums(x, out); }, 3);
  const DataVector x{0.25, -1.5, 3.0};
  std::vector<double> expected(3);
  weightedSums(x, expected);

  for (const VectorSourceFunc* f : {&byCopy, &byMove}) {
    BOOST_CHECK_EQUAL(f->nOutputs(), 3u);
    std::vector<double> out(3, -1.0);
    f->evaluate(x, out);
    BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), expected.begin(), expected.end());
  }
}

BOOST_AUTO_TEST_CASE(one_output) {
  const VectorSourceFunc f(weightedSums, 1);
  const DataVector x{2.0};
  double out = 0.0;

  BOOST_CHECK_EQUAL(f.nOutputs(), 1u);
  f.evaluate(x, std::span<double>(&out, 1));
  BOOST_CHECK_EQUAL(out, 2.0);
}

BOOST_AUTO_TEST_CASE(copies_call_copies_of_the_callable) {
  // As for SourceFunc: state captured by value is not shared between copies
  const VectorSourceFunc original(
      [count = 0](const DataVector&, std::span<double> out) mutable { out[0] = ++count; }, 1);
  const VectorSourceFunc copy(original);
  const DataVector x{0.5};
  double out = 0.0;

  original.evaluate(x, std::span<double>(&out, 1));
  BOOST_CHECK_EQUAL(out, 1.0);
  original.evaluate(x, std::span<double>(&out, 1));
  BOOST_CHECK_EQUAL(out, 2.0);
  copy.evaluate(x, std::span<double>(&out, 1));
  BOOST_CHECK_EQUAL(out, 1.0);
}

BOOST_AUTO_TEST_CASE(evaluate_normalized_maps_onto_the_domain) {
  const VectorSourceFunc f(weightedSums, 2);

  for (const size_t nDim : {1, 3}) {
    HyperCubeArea domain(nDim);
    DataVector unit(nDim);
    DataVector mapped(nDim);
    for (size_t i = 0; i < nDim; i++) {
      domain[i] = {-0.5 + 0.1 * static_cast<double>(i), 1.0 + 0.2 * static_cast<double>(i)};
      unit[i] = 0.125 + 0.25 * static_cast<double>(i);
      mapped[i] = unit[i] * (domain[i].second - domain[i].first) + domain[i].first;
    }
    std::vector<double> expected(2);
    weightedSums(mapped, expected);

    std::vector<double> out(2);
    f.evaluateNormalizedInPlace(unit, domain, out);
    for (size_t i = 0; i < nDim; i++) {
      BOOST_CHECK_CLOSE(unit[i], mapped[i], 1e-13);  // overwritten with the mapped point
    }
    for (size_t k = 0; k < 2; k++) {
      BOOST_CHECK_CLOSE(out[k], expected[k], 1e-12);
    }
  }
}

BOOST_AUTO_TEST_CASE(zero_outputs_are_rejected) {
  BOOST_CHECK_THROW(VectorSourceFunc(weightedSums, 0), sgpp::base::data_exception);
}

BOOST_AUTO_TEST_SUITE_END()
