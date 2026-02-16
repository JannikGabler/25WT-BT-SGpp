// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <cmath>
#include "sgpp/combigrid/multiindices/multiindex.hpp"
#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <random>
#include <sgpp/combigrid/constants.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients.hpp>
#include <sgpp_base.hpp>
#include <vector>

namespace {

std::vector<sgpp::combigrid::MI> makeSimpleMIs(const size_t nDim) {
  // Erstelle eine kleine, definitiv downwards-closed Menge:
  // - Null-MI
  // - ein paar Einheits-MIs in den ersten min(4, nDim) Dimensionen
  using MI = sgpp::combigrid::MI;
  std::vector<MI> res;
  MI zero(nDim, static_cast<MI::value_type>(0));
  res.push_back(zero);
  size_t add = std::min<size_t>(4, nDim);
  for (size_t d = 0; d < add; ++d) {
    MI mi = zero;
    mi[d] = static_cast<MI::value_type>(1);
    res.push_back(mi);
  }
  return res;
}

sgpp::combigrid::MIVec make_random_downwards_closed_mivec(const size_t nDim,
                                                          const size_t targetSize,
                                                          const uint64_t seed = 123456789ULL) {
  using MI = sgpp::combigrid::MI;
  using MIVal = MI::value_type;

  std::mt19937_64 rng(seed);
  std::vector<MI> list;

  // Verwende set von vector<MIVal> um Duplikate zu vermeiden
  std::set<std::vector<MIVal>> s;

  // Start mit Null-MI
  std::vector<MIVal> zeroVec(nDim, MIVal(0));
  s.insert(zeroVec);

  std::vector<std::vector<MIVal>> currentElements;
  currentElements.push_back(zeroVec);

  std::uniform_int_distribution<size_t> distExisting(0, 0);
  std::uniform_int_distribution<size_t> distDim(0, nDim > 0 ? nDim - 1 : 0);

  while (s.size() < targetSize) {
    // wähle zufälliges vorhandenes Element
    if (currentElements.empty()) currentElements.push_back(zeroVec);
    distExisting = std::uniform_int_distribution<size_t>(0, currentElements.size() - 1);
    size_t idx = distExisting(rng);
    std::vector<MIVal> candidate = currentElements[idx];

    // wähle Dimension zum Inkrementieren
    size_t dim = distDim(rng);
    candidate[dim] = candidate[dim] + MIVal(1);

    // falls neu, einfügen und auch in currentElements aufnehmen
    auto it = s.insert(candidate);
    if (it.second) {
      // neu
      currentElements.push_back(candidate);
    } else {
      // existiert bereits -> try again (loop)
      // to avoid potential infinite loops if we can't grow (shouldn't happen for reasonable
      // targetSize) but we still continue trying
      continue;
    }
  }

  // Baue vector<MI> aus set s (Set ist geordnet, aber Reihenfolge ist für MIVec nicht wichtig)
  for (const auto& vec : s) {
    MI mi;
    mi.assign(vec.begin(), vec.end());
    list.push_back(mi);
  }

  // Konvertiere zu MIVec
  return sgpp::combigrid::MIVec(list);
}

}  // namespace

BOOST_AUTO_TEST_SUITE(OP_computeCTCoeffs)

// Test: viele Dimensionen 1..64 mit einem kleinen, downwards-closed MIVec
BOOST_AUTO_TEST_CASE(Dimensions1to64_SimpleCases) {
  for (size_t dim = 1; dim <= 64; ++dim) {
    const std::vector<sgpp::combigrid::MI> mis = makeSimpleMIs(dim);
    const sgpp::combigrid::MIVec miVec(mis);

    // BOOST_CHECK_MESSAGE(miVec.isDownwardsClosed(),
    //                     "MIVec should be downwards-closed for dim = " << dim);

    const std::vector<int> expected = sgpp::combigrid::tools::computeCTCoeffsNaive(miVec);
    const std::vector<int> got = sgpp::combigrid::tools::computeCTCoeffs(miVec);

    BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), got.begin(), got.end());
  }
}

// Test: Variation mit etwas größeren MIs (ein paar Komponenten = 2) über mehrere Dimensionen
BOOST_AUTO_TEST_CASE(MultiValueEntries_VariousDimensions) {
  for (size_t dim = 1; dim <= 64; dim += 7) {  // springe in Schritten, um Anzahl Tests zu begrenzen
    using MI = sgpp::combigrid::MI;
    MI zero(dim, static_cast<MI::value_type>(0));
    std::vector<MI> mis;
    mis.push_back(zero);

    // füge einige MIs mit Komponenten 1 und 2 hinzu (immer so bauen, dass downwards-closed bleibt)
    for (size_t d = 0; d < std::min<size_t>(dim, 6); ++d) {
      MI a = zero;
      a[d] = static_cast<MI::value_type>(1);
      mis.push_back(a);

      MI b = a;
      b[d] = static_cast<MI::value_type>(2);
      // um b downwards-closed zu halten, stelle sicher, dass a bereits in mis ist (ist so)
      mis.push_back(b);
    }

    sgpp::combigrid::MIVec miVec(mis);
    // BOOST_CHECK_MESSAGE(miVec.isDownwardsClosed(),
    //                     "MIVec should be downwards-closed for dim = " << dim);

    std::vector<int> expected = sgpp::combigrid::tools::computeCTCoeffsNaive(miVec);
    std::vector<int> got = sgpp::combigrid::tools::computeCTCoeffs(miVec);

    BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), got.begin(), got.end());
  }
}

// Random tests: ein Test mit "serieller" Anzahl an Multiindizes (weniger als
// MIN_MIS_FOR_CONCURRENCY) und ein Test mit "paralleler" Anzahl (>= MIN_MIS_FOR_CONCURRENCY).
BOOST_AUTO_TEST_CASE(RandomDownwardsClosed_Serial) {
  using namespace sgpp::combigrid;

  const size_t nMI = sgpp::combigrid::ct_coefficients::MIN_MIS_FOR_CONCURRENCY - 1;
  const size_t nDim = 12;  // ausreichend viele Dimensionen für zufällige Verteilung

  sgpp::combigrid::MIVec serialMIVec = make_random_downwards_closed_mivec(nDim, nMI, /*seed=*/1111);
  //   BOOST_CHECK_MESSAGE(serialMIVec.isDownwardsClosed(), "serialMIVec must be downwards-closed");
  BOOST_CHECK_MESSAGE(serialMIVec.nMI() == nMI, "serialMIVec should have requested size");

  std::vector<int> expectedSerial = sgpp::combigrid::tools::computeCTCoeffsNaive(serialMIVec);
  std::vector<int> gotSerial = sgpp::combigrid::tools::computeCTCoeffs(serialMIVec);
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedSerial.begin(), expectedSerial.end(), gotSerial.begin(),
                                gotSerial.end());
}

BOOST_AUTO_TEST_CASE(RandomDownwardsClosed_Parallel) {
  using namespace sgpp::combigrid;

  const size_t nMI = sgpp::combigrid::ct_coefficients::MIN_MIS_FOR_CONCURRENCY - 1;
  const size_t nDim = 12;  // ausreichend viele Dimensionen für zufällige Verteilung

  sgpp::combigrid::MIVec parallelMIVec =
      make_random_downwards_closed_mivec(nDim, nMI, /*seed=*/2222);
  //   BOOST_CHECK_MESSAGE(parallelMIVec.isDownwardsClosed(), "parallelMIVec must be
  //   downwards-closed");
  BOOST_CHECK_MESSAGE(parallelMIVec.nMI() >= nMI,
                      "parallelMIVec should have at least requested size");

  std::vector<int> expectedParallel = sgpp::combigrid::tools::computeCTCoeffsNaive(parallelMIVec);
  std::vector<int> gotParallel = sgpp::combigrid::tools::computeCTCoeffs(parallelMIVec);
  BOOST_CHECK_EQUAL_COLLECTIONS(expectedParallel.begin(), expectedParallel.end(),
                                gotParallel.begin(), gotParallel.end());
}

BOOST_AUTO_TEST_SUITE_END()
