// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/tools/operators/interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

std::vector<double> naiveBarycentricWeights(const std::vector<double>& nodes) {
  const size_t n = nodes.size();
  std::vector<double> w(n, 0.0);
  for (size_t j = 0; j < n; ++j) {
    long double prod = 1.0L;
    for (size_t k = 0; k < n; ++k) {
      if (k == j) continue;
      prod *= (static_cast<long double>(nodes[j]) - static_cast<long double>(nodes[k]));
    }
    long double val = 1.0L / prod;  // kann +/-inf oder nan produzieren
    w[j] = static_cast<double>(val);
  }
  return w;
}

static void checkWeightsEqualUpToScale(const std::vector<double>& expected,
                                       const std::vector<double>& actual,
                                       long double rel_tol = 1e-12L, long double abs_tol = 1e-15L) {
  BOOST_REQUIRE_EQUAL(expected.size(), actual.size());
  const size_t n = expected.size();
  if (n == 0) return;

  // Prüfe einfache Konsistenz: wenn erwartete Komponente endlich ist, sollte actual ebenfalls
  // (außer bei Duplikaten/NaN/Inf-Fällen)
  bool expected_has_finite = false;
  for (size_t i = 0; i < n; ++i)
    if (std::isfinite(expected[i])) {
      expected_has_finite = true;
      break;
    }

  // Falls keine finiten erwarteten Werte (sehr ungewöhnlich), akzeptiere, dass actual mindestens
  // eine nicht-finite Komponente hat
  if (!expected_has_finite) {
    bool any_nonfinite = false;
    for (double v : actual)
      if (!std::isfinite(v)) {
        any_nonfinite = true;
        break;
      }
    BOOST_CHECK_MESSAGE(any_nonfinite,
                        "Erwartete alle nicht-finiten Werte; actual sollte mindestens eine "
                        "nicht-finite Komponente enthalten.");
    return;
  }

  // Wähle Normfaktor: L_infty (max absolute) über finiten Komponenten
  long double max_abs_e = 0.0L;
  long double max_abs_a = 0.0L;
  for (size_t i = 0; i < n; ++i) {
    if (std::isfinite(expected[i]))
      max_abs_e = std::max(max_abs_e, fabsl((long double)expected[i]));
    if (std::isfinite(actual[i])) max_abs_a = std::max(max_abs_a, fabsl((long double)actual[i]));
  }

  // Falls eine der Normen 0 (alle erwarteten sind 0?), versuche eine nicht-null Komponente als
  // Referenz
  size_t ref = n;  // index of reference
  if (max_abs_e <= 0.0L) {
    for (size_t i = 0; i < n; ++i) {
      if (std::isfinite(expected[i]) && fabsl((long double)expected[i]) > 0.0L) {
        ref = i;
        break;
      }
    }
  } else {
    // wähle ref als index zu max_abs_e (für bessere numerische Stabilität)
    for (size_t i = 0; i < n; ++i) {
      if (std::isfinite(expected[i]) && fabsl((long double)expected[i]) == max_abs_e) {
        ref = i;
        break;
      }
    }
  }

  // Falls wir immer noch kein Ref gefunden haben (z. B. expected enthält nur ±0.0), suche
  // irgendeine finite Komponente
  if (ref == n) {
    for (size_t i = 0; i < n; ++i) {
      if (std::isfinite(expected[i])) {
        ref = i;
        break;
      }
    }
  }

  // Wenn es keine finite expected-Komponente gäbe, wäre das oben schon behandelt. Hier ist ref
  // gültig.
  BOOST_REQUIRE_MESSAGE(ref < n, "Keine geeignete Referenz-Komponente in expected gefunden.");

  // Die Idee: vergleiche die Verhältnisse expected[i]/expected[ref] mit actual[i]/actual[ref].
  double e_ref = expected[ref];
  double a_ref = actual[ref];

  // Wenn actual[ref] nicht-finite oder 0 obwohl expected[ref] nicht 0 -> Fehler (außer
  // Spezialfälle)
  if (!std::isfinite(a_ref) || (a_ref == 0.0 && e_ref != 0.0)) {
    BOOST_FAIL(
        "Referenz-Komponente in actual ist nicht-finite oder 0, obwohl expected Referenz != 0. "
        "Möglicher Hinweis auf Duplikate oder numerische Probleme.");
  }

  for (size_t i = 0; i < n; ++i) {
    double e = expected[i];
    double a = actual[i];

    // Fälle mit nicht-finiten erwarteten Werten: akzeptiere, dass actual die gleiche Art von
    // Nicht-Finitheit hat
    if (!std::isfinite(e)) {
      if (std::isnan(e)) {
        BOOST_CHECK(std::isnan(a));
      } else {
        // e ist +/-inf: a sollte +/-inf in derselben Richtung sein (Skalar m kann auch +/-inf sein
        // — das wird schwer, aber wir prüfen Grundkonsistenz)
        BOOST_CHECK(std::isinf(a));
        if (std::isinf(a)) {
          BOOST_CHECK(std::signbit(e) == std::signbit(a));
        }
      }
      continue;
    }

    // Erwartetes e ist endlich; dann sollte actual a endlich sein
    BOOST_CHECK_MESSAGE(std::isfinite(a), "expected[" << i << "] ist endlich, aber actual[" << i
                                                      << "] ist nicht-finite.");

    // Wenn e_ref ist 0 (selten), dann vergleichen wir absolute Toleranz auf die Differenz a -
    // (a_ref/e_ref)*e ist nicht möglich.
    if (e_ref == 0.0) {
      // fallback: normalisiere global über max_abs_e / max_abs_a falls möglich
      if (max_abs_e > 0.0L && max_abs_a > 0.0L) {
        long double en = static_cast<long double>(e) / max_abs_e;
        long double an = static_cast<long double>(a) / max_abs_a;
        long double diff = fabsl(en - an);
        BOOST_CHECK_MESSAGE(diff <= rel_tol,
                            "weight[" << i << "] normalisierte Differenz (Linf-Norm) zu groß: diff="
                                      << diff << " expected_norm=" << en << " actual_norm=" << an);
      } else {
        // as last resort use absolute tolerance
        long double diff = fabsl((long double)a - (long double)e);
        BOOST_CHECK_MESSAGE(diff <= abs_tol,
                            "weight[" << i << "] absolute Differenz zu groß (fallback): abs="
                                      << diff << " expected=" << e << " actual=" << a);
      }
      continue;
    }

    // Vergleiche Verhältnisse Ri = e_i / e_ref und Si = a_i / a_ref
    long double Ri = static_cast<long double>(e) / static_cast<long double>(e_ref);
    long double Si = static_cast<long double>(a) / static_cast<long double>(a_ref);
    long double diff = fabsl(Ri - Si);
    long double denom = fabsl(Ri);
    if (denom > 0.0L) {
      long double rel = diff / denom;
      BOOST_CHECK_MESSAGE(rel <= rel_tol,
                          "weight[" << i << "] Verhältnis-Fehler zu groß: rel=" << rel
                                    << " expected_ratio=" << Ri << " actual_ratio=" << Si);
    } else {
      // Ri nahe 0 -> check absolute
      BOOST_CHECK_MESSAGE(diff <= abs_tol,
                          "weight[" << i << "] Verhältnis-Fehler (Ri~0) zu groß: abs=" << diff
                                    << " expected_ratio=" << Ri << " actual_ratio=" << Si);
    }

    // zusätzlich: erlauben globale Vorzeichen-Umkehr (falls m<0)
    // prüfe auch gegen -Si (nur falls oben fehlgeschlagen)
    if (fabsl(Ri - Si) > rel_tol * (denom > 0.0L ? denom : 1.0L)) {
      long double diff_neg = fabsl(Ri + Si);
      if (denom > 0.0L) {
        long double rel_neg = diff_neg / denom;
        BOOST_CHECK_MESSAGE(rel_neg <= rel_tol,
                            "weight[" << i
                                      << "] Verhältnis-Fehler (auch gegen globale "
                                         "Vorzeichen-Umkehr geprüft) zu groß: rel_neg="
                                      << rel_neg << " expected_ratio=" << Ri
                                      << " actual_ratio_neg=" << -Si);
      } else {
        BOOST_CHECK_MESSAGE(
            diff_neg <= abs_tol,
            "weight[" << i << "] Verhältnis-Fehler (Ri~0, Vorzeichenumkehr) zu groß: abs_neg="
                      << diff_neg);
      }
    }
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(barycentric_weights)

BOOST_AUTO_TEST_CASE(two_nodes_simple) {
  const std::vector<double> nodes = {0.0, 1.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-15L, 1e-15L);

  BOOST_REQUIRE_EQUAL(actual.size(), 2);

  long double sum = static_cast<long double>(actual[0]) + static_cast<long double>(actual[1]);
  long double maxabs = std::max(fabsl((long double)actual[0]), fabsl((long double)actual[1]));
  if (maxabs > 0.0L) {
    BOOST_CHECK_MESSAGE(fabsl(sum) / maxabs <= 1e-12L,
                        "actual[0] + actual[1] should be approximately 0.");
  } else {
    BOOST_CHECK_MESSAGE(fabsl(sum) <= 1e-12L, "actual[0] + actual[1] should be approximately 0.");
  }
}

BOOST_AUTO_TEST_CASE(two_nodes_negative) {
  const std::vector<double> nodes = {-2.0, 3.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-15L, 1e-15L);

  BOOST_REQUIRE_EQUAL(actual.size(), 2);

  long double ratio = static_cast<long double>(actual[0]) / static_cast<long double>(-actual[1]);
  BOOST_CHECK_MESSAGE(std::isfinite((double)ratio) && fabsl(ratio - 1.0L) <= 1e-12L,
                      "actual[0] should approximately equal -actual[1].");
}

BOOST_AUTO_TEST_CASE(three_nodes_fixed) {
  const std::vector<double> nodes = {-1.0, 0.0, 1.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-14L, 1e-15L);
}

BOOST_AUTO_TEST_CASE(equidistant_four_nodes) {
  const std::vector<double> nodes = {0.0, 1.0, 2.0, 3.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-14L, 1e-15L);

  bool alt_signs = true;
  for (size_t i = 1; i < actual.size(); ++i) {
    if (std::isfinite(actual[i - 1]) && std::isfinite(actual[i])) {
      if (!(std::signbit(actual[i - 1]) != std::signbit(actual[i]))) {
        alt_signs = false;
        break;
      }
    } else {
      alt_signs = false;
      break;
    }
  }
  BOOST_CHECK_MESSAGE(alt_signs, "The sign of weights is not alternating.");
}

BOOST_AUTO_TEST_CASE(symmetric_four_nodes) {
  const std::vector<double> nodes = {-2.0, -1.0, 1.0, 2.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-14L, 1e-15L);

  // Symmetrie-check: expected[0] should be proportional to expected[3] and
  // expected[1] should be proportional to expected[2]
  if (actual.size() == 4) {
    // Compare ratios actual[0]/actual[3] and expected[0]/expected[3]
    if (std::isfinite(actual[0]) && std::isfinite(actual[3]) && std::isfinite(expected[0]) &&
        std::isfinite(expected[3])) {
      const long double ratio_a =
          static_cast<long double>(actual[0]) / static_cast<long double>(actual[3]);
      const long double ratio_e =
          static_cast<long double>(expected[0]) / static_cast<long double>(expected[3]);
      BOOST_CHECK_MESSAGE(fabsl(ratio_a - ratio_e) <= 1e-12L * fabsl(ratio_e),
                          "Symmetrie relation between 0 and 3 does not hold.");
    }
    if (std::isfinite(actual[1]) && std::isfinite(actual[2]) && std::isfinite(expected[1]) &&
        std::isfinite(expected[2])) {
      long double ratio_a =
          static_cast<long double>(actual[1]) / static_cast<long double>(actual[2]);
      long double ratio_e =
          static_cast<long double>(expected[1]) / static_cast<long double>(expected[2]);
      BOOST_CHECK_MESSAGE(fabsl(ratio_a - ratio_e) <= 1e-12L * fabsl(ratio_e),
                          "Symmetrie relation between 1 and 2 does not hold.");
    }
  }
}

BOOST_AUTO_TEST_CASE(rational_three_nodes) {
  const std::vector<double> nodes = {1.5, 2.5, 4.0};

  const auto expected = naiveBarycentricWeights(nodes);
  const auto actual = tools::barycentricFormulaWeights(nodes);

  checkWeightsEqualUpToScale(expected, actual, 1e-14L, 1e-15L);
}

BOOST_AUTO_TEST_CASE(random_unique_nodes_medium) {
  const size_t nNodes = 30;

  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    std::vector<double> nodes;
    nodes.reserve(nNodes);

    while (nodes.size() < nNodes) {
      const double x = randGen.getUniformRN(-1000, 1000);

      bool dup = false;
      for (double y : nodes) {
        if (std::fabs(x - y) < 1e-12) {
          dup = true;
          break;
        }
      }
      if (!dup) nodes.push_back(x);
    }

    const auto expected = naiveBarycentricWeights(nodes);
    const auto actual = tools::barycentricFormulaWeights(nodes);

    checkWeightsEqualUpToScale(expected, actual, 1e-11L, 1e-13L);
  }
}

// BOOST_AUTO_TEST_CASE(duplicate_nodes_behavior) {
//   std::vector<double> nodes = {0.0, 1.0, 1.0};  // duplicate 1.0

//   try {
//     const auto w = tools::barycentricFormulaWeights(nodes);

//     bool has_bad = false;
//     for (double v : w) {
//       if (!std::isfinite(v)) {
//         has_bad = true;
//         break;
//       }
//     }
//     BOOST_CHECK_MESSAGE(
//         has_bad,
//         "There is no NaN / infinite value or exception thrown when having duplicate nodes.");
//   } catch (const std::exception& ex) {
//     BOOST_CHECK(true);
//   } catch (...) {
//     BOOST_CHECK(true);
//   }
// }

// BOOST_AUTO_TEST_CASE(nan_inf_nodes_behavior) {
//   std::vector<double> nodes = {0.0, std::numeric_limits<double>::infinity(), std::nan("")};

//   try {
//     const auto w = tools::barycentricFormulaWeights(nodes);

//     bool any_nonfinite = false;
//     for (double v : w)
//       if (!std::isfinite(v)) {
//         any_nonfinite = true;
//         break;
//       }
//     BOOST_CHECK_MESSAGE(any_nonfinite, "At least one weight should be NaN / infinite.");
//   } catch (...) {
//     BOOST_CHECK(true);
//   }
// }

BOOST_AUTO_TEST_SUITE_END()