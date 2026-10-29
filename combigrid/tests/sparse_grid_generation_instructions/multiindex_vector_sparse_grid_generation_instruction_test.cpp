/**
 * @file multiindex_vector_sparse_grid_generation_instruction_test.cpp
 * @brief Boost.Test cases for @c MIVecSGGenInstr (sparse-grid generation
 * from a user-supplied multi-index set).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/context.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/sparse_grid_generation/extensive_sparse_grid_generation.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <string>
#include <utility>
#include <vector>

using namespace sgpp::combigrid;

static sgpp::base::RandomNumberGenerator& randGen =
    sgpp::base::RandomNumberGenerator::getInstance();

namespace {

// Hilfs: konvertiert MI -> std::vector<LvlType>
std::vector<LvlType> toStdVec(const LvlMI& mi) {
  return std::vector<LvlType>(mi.begin(), mi.end());
}

// Prüft, ob ein MIVec Duplikate enthält
bool hasDuplicates(const LvlMIVec& v) {
  std::set<std::vector<LvlType>> s;
  for (size_t i = 0; i < v.nMI(); ++i) {
    s.insert(toStdVec(v[i]));
  }
  return (s.size() != v.nMI());
}

// Vergleicht zwei MIVecs als Mengen (Reihenfolge ignoriert)
bool equalAsSets(const LvlMIVec& a, const LvlMIVec& b) {
  if (a.nDim() != b.nDim() || a.nMI() != b.nMI()) return false;
  std::multiset<std::vector<LvlType>> sa, sb;
  for (size_t i = 0; i < a.nMI(); ++i) sa.insert(toStdVec(a[i]));
  for (size_t i = 0; i < b.nMI(); ++i) sb.insert(toStdVec(b[i]));
  return sa == sb;
}

// Bruteforce: komponentenweise downwards-closure eines gegebenen MIVec
LvlMIVec computeDownwardClosure(const LvlMIVec& input) {
  const size_t nDim = input.nDim();
  std::set<std::vector<LvlType>> setMi;

  for (size_t miIdx = 0; miIdx < input.nMI(); ++miIdx) {
    const LvlMI base = input[miIdx];

    std::vector<LvlType> cur(nDim, 0);

    std::function<void(size_t)> rec = [&](size_t d) {
      if (d == nDim) {
        setMi.insert(cur);
        return;
      }
      for (LvlType v = 0; v <= base[d]; ++v) {
        cur[d] = v;
        rec(d + 1);
      }
    };

    rec(0);
  }

  // Konvertiere set -> vector<MI> -> MIVec
  std::vector<LvlMI> vecMi;
  vecMi.reserve(setMi.size());
  for (const auto& v : setMi) {
    vecMi.emplace_back(v.begin(), v.end());
  }

  LvlMIVec out(vecMi);
  return out;
}

// Colexicographic order (last dimension most significant): the output order of the closure.
bool colexLess(const std::vector<LvlType>& a, const std::vector<LvlType>& b) {
  return std::lexicographical_compare(a.rbegin(), a.rend(), b.rbegin(), b.rend());
}

// Expected output of genFull/ReducedMIVecWithCoeffs: the closure in colexicographic order, its
// coefficients, optionally without zero coefficients, stably partitioned (zero coefficients first).
std::pair<LvlMIVec, std::vector<CTCoeffType>> expectedGeneratorOutput(const LvlMIVec& input,
                                                                      const bool includeZeros) {
  const LvlMIVec setClosure = computeDownwardClosure(input);
  std::vector<std::vector<LvlType>> sorted;
  for (size_t i = 0; i < setClosure.nMI(); ++i) sorted.push_back(toStdVec(setClosure[i]));
  std::sort(sorted.begin(), sorted.end(), colexLess);

  LvlMIVec closure(input.nDim(), sorted.size());
  for (size_t i = 0; i < sorted.size(); ++i) closure.setMI(i, sorted[i]);
  const std::vector<CTCoeffType> coeffs = tools::computeCTCoeffs(closure);

  LvlMIVec mis(input.nDim(), 0);
  std::vector<CTCoeffType> kept;
  for (size_t i = 0; i < closure.nMI(); ++i) {
    if (includeZeros || coeffs[i] != 0) {
      mis.push_back(closure[i]);
      kept.push_back(coeffs[i]);
    }
  }
  tools::extensive_sparse_grid_generation::partitionForExtensiveSG(mis, kept);
  return {std::move(mis), std::move(kept)};
}

// Checks MIs (including their order) and coefficients of a generator output.
void checkSameSequence(const std::pair<LvlMIVec, std::vector<CTCoeffType>>& actual,
                       const std::pair<LvlMIVec, std::vector<CTCoeffType>>& expected) {
  BOOST_REQUIRE_EQUAL(actual.first.nDim(), expected.first.nDim());
  BOOST_REQUIRE_EQUAL(actual.first.nMI(), expected.first.nMI());
  BOOST_REQUIRE_EQUAL(actual.second.size(), expected.second.size());
  for (size_t i = 0; i < expected.first.nMI(); ++i) {
    BOOST_REQUIRE_MESSAGE(actual.first[i] == expected.first[i], "MI differs at position " << i);
    BOOST_REQUIRE_MESSAGE(actual.second[i] == expected.second[i],
                          "Coefficient differs at position " << i);
  }
}

}  // namespace

// The order of the generated multi-indices (and thus of the tensor grids) is observable: pin it.
BOOST_AUTO_TEST_CASE(OutputOrderIsColexClosureOrder) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    for (size_t rep = 0; rep < 5; ++rep) {
      const size_t nDim = 1 + randGen.getUniformIndexRN(6);
      const size_t nInput = 1 + randGen.getUniformIndexRN(12);
      const LvlType maxComp = static_cast<LvlType>(randGen.getUniformIndexRN(5));

      LvlMIVec input(nDim, nInput);
      for (size_t i = 0; i < nInput; ++i) {
        for (size_t d = 0; d < nDim; ++d) {
          input(i, d) = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));
        }
      }

      BOOST_TEST_CONTEXT("nDim = " << nDim << ", nInput = " << nInput) {
        const MIVecSGGenInstr instr(input);
        checkSameSequence(instr.genFullMIVecWithCoeffs(), expectedGeneratorOutput(input, true));
        checkSameSequence(instr.genReducedMIVecWithCoeffs(),
                          expectedGeneratorOutput(input, false));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(FullInputIsFixedPoint) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    const LvlType maxLvl = static_cast<LvlType>(randGen.getUniformIndexRN(8));
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);

    const CompleteSGGenInstr fsg(maxLvl, nDim);
    const LvlMIVec input = fsg.genReducedMIVec();

    MIVecSGGenInstr instr(input);
    const LvlMIVec result = instr.genReducedMIVec();

    BOOST_CHECK(result.nDim() == input.nDim());
    BOOST_CHECK(result.nMI() == input.nMI());
    BOOST_CHECK(equalAsSets(result, input));
  }
}

BOOST_AUTO_TEST_CASE(RandomInputMatchesBruteforceFiltered) {
  randGen.setSeed();
  BOOST_TEST_CONTEXT("Seed: " + std::to_string(randGen.getSeed())) {
    // Parameter
    const size_t nDim = 1 + randGen.getUniformIndexRN(5);    // 1..5
    const size_t nInput = 1 + randGen.getUniformIndexRN(6);  // 1..6
    const LvlType maxComp = 1 + static_cast<LvlType>(randGen.getUniformIndexRN(6));

    // Create random MIs
    LvlMIVec inputMIVec(nDim, nInput);
    for (size_t i = 0; i < nInput; ++i) {
      std::vector<LvlType> mi(nDim);
      for (size_t d = 0; d < nDim; ++d) {
        mi[d] = static_cast<LvlType>(randGen.getUniformIndexRN(maxComp + 1));  // 0..maxComp
      }
      inputMIVec.setMI(i, mi);
    }

    const MIVecSGGenInstr instr(inputMIVec);

    const auto pairWithCoeff = instr.genReducedMIVecWithCoeffs();
    const LvlMIVec resultMiVec = pairWithCoeff.first;
    const std::vector<CTCoeffType> resultCoeff = pairWithCoeff.second;

    BOOST_CHECK(resultMiVec.nDim() == nDim);
    BOOST_CHECK(!hasDuplicates(resultMiVec));
    BOOST_CHECK(resultMiVec.nMI() == resultCoeff.size());

    const LvlMIVec bruteClosure = computeDownwardClosure(inputMIVec);
    const std::vector<CTCoeffType> bruteCoeffs = tools::computeCTCoeffs(bruteClosure);

    std::map<std::vector<LvlType>, CTCoeffType> expectedMap;
    for (size_t i = 0; i < bruteClosure.nMI(); ++i) {
      const auto miVec = toStdVec(bruteClosure[i]);
      const CTCoeffType c = bruteCoeffs[i];
      if (c != static_cast<CTCoeffType>(0)) {
        expectedMap.emplace(miVec, c);
      }
    }

    std::map<std::vector<LvlType>, CTCoeffType> resultMap;
    for (size_t i = 0; i < resultMiVec.nMI(); ++i) {
      resultMap.emplace(toStdVec(resultMiVec[i]), resultCoeff[i]);
    }

    BOOST_CHECK_MESSAGE(resultMap.size() == expectedMap.size(),
                        "Number of coefficients unequal to 0 are different between the actual and "
                        "expected result.");

    for (const auto& kv : expectedMap) {
      const auto it = resultMap.find(kv.first);
      BOOST_REQUIRE_MESSAGE(it != resultMap.end(), "A MI is missing.");
      BOOST_REQUIRE_MESSAGE(it->second == kv.second, "A coefficient does not match. Expected '"
                                                         << kv.second << "' but got '" << it->second
                                                         << "'.");
    }
  }
}

BOOST_AUTO_TEST_CASE(Deterministic1D_singleMI) {
  const LvlMIVec input({{3}});

  const MIVecSGGenInstr instr(input);
  const auto pairWithCoeff = instr.genReducedMIVecWithCoeffs();
  const LvlMIVec resultMiVec = pairWithCoeff.first;
  const std::vector<CTCoeffType> resultCoeff = pairWithCoeff.second;

  // Expected via brute-force
  const LvlMIVec bruteClosure = computeDownwardClosure(input);
  const std::vector<CTCoeffType> bruteCoeffs = tools::computeCTCoeffs(bruteClosure);

  std::map<std::vector<LvlType>, CTCoeffType> expectedMap;
  for (size_t i = 0; i < bruteClosure.nMI(); ++i) {
    const auto mv = toStdVec(bruteClosure[i]);
    const auto c = bruteCoeffs[i];
    if (c != static_cast<CTCoeffType>(0)) {
      expectedMap.emplace(mv, c);
    }
  }

  std::map<std::vector<LvlType>, CTCoeffType> resultMap;
  for (size_t i = 0; i < resultMiVec.nMI(); ++i) {
    resultMap.emplace(toStdVec(resultMiVec[i]), resultCoeff[i]);
  }

  BOOST_CHECK_EQUAL(expectedMap.size(), resultMap.size());
  for (const auto& kv : expectedMap) {
    const auto it = resultMap.find(kv.first);
    BOOST_REQUIRE_MESSAGE(it != resultMap.end(), "A mi is missing.");
    BOOST_REQUIRE_MESSAGE(it->second == kv.second, "Coefficients do not match. Expected '"
                                                       << kv.second << "' but got '" << it->second
                                                       << "'.");
  }
}

BOOST_AUTO_TEST_CASE(Deterministic2D_twoMIs) {
  const LvlMIVec input({{1, 1}, {2, 0}});

  const MIVecSGGenInstr instr(input);
  const auto pairWithCoeff = instr.genReducedMIVecWithCoeffs();
  const LvlMIVec resultMiVec = pairWithCoeff.first;
  const std::vector<CTCoeffType> resultCoeff = pairWithCoeff.second;

  const LvlMIVec bruteClosure = computeDownwardClosure(input);
  const std::vector<CTCoeffType> bruteCoeffs = tools::computeCTCoeffs(bruteClosure);

  std::map<std::vector<LvlType>, CTCoeffType> expectedMap;
  for (size_t i = 0; i < bruteClosure.nMI(); ++i) {
    const auto mv = toStdVec(bruteClosure[i]);
    const auto c = bruteCoeffs[i];
    if (c != static_cast<CTCoeffType>(0)) {
      expectedMap.emplace(mv, c);
    }
  }

  std::map<std::vector<LvlType>, CTCoeffType> resultMap;
  for (size_t i = 0; i < resultMiVec.nMI(); ++i) {
    resultMap.emplace(toStdVec(resultMiVec[i]), resultCoeff[i]);
  }

  BOOST_CHECK_EQUAL(expectedMap.size(), resultMap.size());
  for (const auto& kv : expectedMap) {
    const auto it = resultMap.find(kv.first);
    BOOST_REQUIRE_MESSAGE(it != resultMap.end(), "A mi is missing.");
    BOOST_REQUIRE_MESSAGE(it->second == kv.second, "Coefficients do not match. Expected '"
                                                       << kv.second << "' but got '" << it->second
                                                       << "'.");
  }
}

BOOST_AUTO_TEST_CASE(Deterministic3D_smallExample) {
  const LvlMIVec input({{1, 0, 1}, {0, 2, 0}});

  const MIVecSGGenInstr instr(input);
  const auto pairWithCoeff = instr.genReducedMIVecWithCoeffs();
  const LvlMIVec resultMiVec = pairWithCoeff.first;
  const std::vector<CTCoeffType> resultCoeff = pairWithCoeff.second;

  const LvlMIVec bruteClosure = computeDownwardClosure(input);
  const std::vector<CTCoeffType> bruteCoeffs = tools::computeCTCoeffs(bruteClosure);

  std::map<std::vector<LvlType>, CTCoeffType> expectedMap;
  for (size_t i = 0; i < bruteClosure.nMI(); ++i) {
    const auto mv = toStdVec(bruteClosure[i]);
    const auto c = bruteCoeffs[i];
    if (c != static_cast<CTCoeffType>(0)) {
      expectedMap.emplace(mv, c);
    }
  }

  std::map<std::vector<LvlType>, CTCoeffType> resultMap;
  for (size_t i = 0; i < resultMiVec.nMI(); ++i) {
    resultMap.emplace(toStdVec(resultMiVec[i]), resultCoeff[i]);
  }

  BOOST_CHECK_EQUAL(expectedMap.size(), resultMap.size());
  for (const auto& kv : expectedMap) {
    const auto it = resultMap.find(kv.first);
    BOOST_REQUIRE_MESSAGE(it != resultMap.end(), "A mi is missing.");
    BOOST_CHECK_MESSAGE(it->second == kv.second, "Coefficients do not match. Expected '"
                                                     << kv.second << "' but got '" << it->second
                                                     << "'.");
  }
}