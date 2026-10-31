// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file interpolation_pinning_test.cpp
 * @brief Boost.Test cases pinning the results of the global (@c interpolate) and the linear
 * (@c interpolateLinear) interpolation of a scalar source function: golden values recorded with
 * the code before WP-29c (equi / cc / cheb1 nodes, boundary offsets 0 / 1, unit and shifted
 * domains, d = 1-4, complete and MIVec grids, a generic query point and one on a node).
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cmath>
#include <cstddef>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Non-polynomial, not separable test function (positive on the test domains).
double testFunc(const DataVector& x) {
  double sqDist = 0.0;
  for (size_t i = 0; i < x.size(); i++) {
    const double c = 0.3 + 0.1 * static_cast<double>(i);
    sqDist += (x[i] - c) * (x[i] - c);
  }
  return std::exp(-sqDist) * (1.0 + 0.5 * x[0]) + 0.25 * x[x.size() - 1];
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

/// One test configuration (complete grid: maximum level 3; else anisotropicMIs).
struct Config {
  std::string nodes;
  LvlType boundary;
  bool unitDomain;
  size_t nDim;
  bool mivec;
};

std::vector<Config> configs(const std::vector<std::string>& nodeNames) {
  std::vector<Config> result;
  for (const std::string& nodes : nodeNames) {
    for (const LvlType boundary : {0u, 1u}) {
      for (const bool unitDomain : {true, false}) {
        for (size_t nDim = 1; nDim <= 4; nDim++) {
          for (const bool mivec : {false, true}) {
            result.push_back({nodes, boundary, unitDomain, nDim, mivec});
          }
        }
      }
    }
  }
  return result;
}

/// The generation instruction of @p config (@p miVec: storage of the MIs, must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const Config& config, LvlMIVec& miVec) {
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.mivec) {
    miVec = anisotropicMIs(config.nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(3, config.nDim);
  }
  genInstr->setNodeGenFunc(nodeGenFuncByName(config.nodes));
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.boundary);
  genInstr->setDomain(config.unitDomain ? HyperCubeArea(config.nDim, {0.0, 1.0})
                                        : shiftedDomain(config.nDim));
  return genInstr;
}

/// Two query points: a generic one and one whose first coordinate is the domain's midpoint (a
/// node of every tensor grid, so that the 1D formulas take their trivial path there).
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

/// Golden values of interpolate, in the order of configs({"equi", "cc", "cheb1"}) x testPoints.
const std::vector<double> GLOBAL_GOLDEN = {
    1.074734838907456,   1.3259867989404039, 1.074734838907456,   1.3259867989404039,
    1.1246751526159016,  1.2847437172585179, 1.124675331134867,   1.2847437262152019,
    1.1738192868293023,  1.3333316205496266, 1.1737260493947355,  1.3332247004691375,
    1.1971123929309189,  1.3520807926273171, 1.1970291394892385,  1.3519860455222017,
    0.52523998455448839, 1.1846910126971426, 0.52523998455448839, 1.1846910126971426,
    0.603674535034598,   1.0969423684018456, 0.60371608051915648, 1.0969422165490295,
    0.71444491696664769, 1.1966904106758798, 0.71478529265549862, 1.1971965757905534,
    0.68796148455510908, 1.0300593930323763, 0.68910218060127126, 1.0309113346900169,
    1.074734838907456,   1.3259867989404039, 1.074734838907456,   1.3259867989404039,
    1.1246751514934248,  1.2847437172585177, 1.1246882692028184,  1.2847437262152019,
    1.1738192314601765,  1.3333316205248482, 1.1737248118054942,  1.3332247341931258,
    1.1971121606146988,  1.3520807834000137, 1.1970291184129811,  1.3519860608730583,
    0.52523998455448839, 1.1846910126971426, 0.52523998455448839, 1.1846910126971426,
    0.60367447949454822, 1.0969423684018458, 0.60379507013336864, 1.0969422165490295,
    0.71446022182795776, 1.1966903808020337, 0.71485702360710568, 1.1971665566099272,
    0.68780086058686574, 1.0300706141490528, 0.6912227776998392,  1.0309818964799775,
    1.0747348389074527,  1.3259867989404039, 1.0747348389074527,  1.3259867989404039,
    1.1246751649569713,  1.2847437172585179, 1.1246770197543021,  1.2847436516424489,
    1.1738193001443942,  1.333331624054749,  1.1735848859982438,  1.3330612621213698,
    1.1971123797789418,  1.3520807649944291, 1.1969013524184242,  1.3518361307202988,
    0.52523998455618126, 1.1846910126971426, 0.52523998455618126, 1.1846910126971426,
    0.60367211777959029, 1.0969423684014812, 0.60357701464079172, 1.0969434436639536,
    0.7144399198792224,  1.1966891032192561, 0.71502514128037564, 1.1979575161266833,
    0.687990567725558,   1.030044988246227,  0.68846097860519795, 1.0317270934270184,
    1.0747348389074527,  1.3259867989404039, 1.0747348389074527,  1.3259867989404039,
    1.1246751732577094,  1.2847437172585177, 1.1247112507103882,  1.2847436516424489,
    1.1738191661534783,  1.3333316242803721, 1.1735825587911817,  1.3330613304659038,
    1.1971117277408097,  1.352080749320915,  1.1969191262532541,  1.3518360557371667,
    0.52523998455618126, 1.1846910126971426, 0.52523998455618126, 1.1846910126971426,
    0.60367162200369207, 1.0969423684014814, 0.60309698334643347, 1.0969434436639536,
    0.71448080803777936, 1.1966888405005147, 0.71450606022324314, 1.1978992932219383,
    0.68841988266812582, 1.0300759150830539, 0.68606427832433636, 1.0313940199144915,
    1.0747348389074538,  1.3259867989404039, 1.0747348389074538,  1.3259867989404039,
    1.1246752184575306,  1.284743717258519,  1.1246785800648287,  1.284743552345625,
    1.1738193542339197,  1.33333162991551,   1.1734499841645938,  1.3329051062067425,
    1.1971124138049112,  1.352080700727029,  1.1967792841079783,  1.351693001012727,
    0.52523998455451815, 1.1846910126971426, 0.52523998455451815, 1.1846910126971426,
    0.60366277317180372, 1.0969423684011521, 0.60345671465781603, 1.096945034657808,
    0.71442211240262588, 1.1966871377145245, 0.71526199597066875, 1.1986607400249325,
    0.6879299901754089,  1.0300155485687832, 0.68797733659523019, 1.0325047336960136,
    1.0747348389074538,  1.3259867989404039, 1.0747348389074538,  1.3259867989404039,
    1.1246752392755368,  1.2847437172585192, 1.124733164106225,   1.284743552345625,
    1.1738191494957342,  1.3333316304654219, 1.1734466015420606,  1.332905208108538,
    1.1971113650210092,  1.3520806793239648, 1.1968138726355007,  1.3516928402155377,
    0.52523998455451815, 1.1846910126971426, 0.52523998455451815, 1.1846910126971426,
    0.60366201490865268, 1.0969423684011519, 0.60246276524141607, 1.096945034657808,
    0.71448516043467758, 1.1966866705478012, 0.7142083068806887,  1.1985773549434491,
    0.68885704943498982, 1.030061948853044,  0.68154020935736692, 1.031814568430278,
};

/// Golden values of interpolateLinear (same order).
const std::vector<double> LINEAR_GOLDEN = {
    1.0742158842947549,  1.3259867989404039, 1.0742158842947549,  1.3259867989404039,
    1.1229235196294491,  1.2836034855848728, 1.1204736314276196,  1.2816268776602311,
    1.1712907580065235,  1.3316001114823246, 1.1530128796456398,  1.3108822783758278,
    1.1932853395353669,  1.3489920981266008, 1.1678980476306795,  1.3202831380094813,
    0.52551648548917407, 1.1846910126971426, 0.52551648548917407, 1.1846910126971426,
    0.60294339955480969, 1.0946730239966134, 0.60111996993926697, 1.091054300514247,
    0.71264331639105172, 1.1922988435412958, 0.68995556982152106, 1.149424058708294,
    0.68597866509496108, 1.0255935221573647, 0.66448051149686371, 0.98427347264247755,
    1.0742158842947549,  1.3259867989404039, 1.0742158842947549,  1.3259867989404039,
    1.1229235196294491,  1.2836034855848728, 1.1204736314276196,  1.2816268776602311,
    1.1712907580065235,  1.3316001114823246, 1.1530128796456398,  1.3108822783758278,
    1.1932853395353669,  1.3489920981266008, 1.1678980476306795,  1.3202831380094813,
    0.52551648548917407, 1.1846910126971426, 0.52551648548917407, 1.1846910126971426,
    0.60294339955480969, 1.0946730239966134, 0.60111996993926697, 1.091054300514247,
    0.71264331639105172, 1.1922988435412958, 0.68995556982152106, 1.149424058708294,
    0.68597866509496108, 1.0255935221573647, 0.66448051149686371, 0.98427347264247755,
    1.0743405895206195,  1.3259867989404039, 1.0743405895206195,  1.3259867989404039,
    1.1218706117982236,  1.2821906010723363, 1.1169569264399697,  1.2776029202955845,
    1.167901496854562,   1.3274795256648513, 1.1343728569630767,  1.288694441290068,
    1.18912732863769,    1.3441597670003853, 1.1439916231995126,  1.2919983031375539,
    0.52555586674826027, 1.1846910126971426, 0.52555586674826027, 1.1846910126971426,
    0.60121118469659351, 1.0917758213141477, 0.59559979339073887, 1.0816344095885386,
    0.70721724131423935, 1.1827045841603281, 0.66856607747089558, 1.1103209349514938,
    0.68067133175700456, 1.0160641533162935, 0.64621319261920074, 0.95159965483556319,
    1.0743405895206195,  1.3259867989404039, 1.0743405895206195,  1.3259867989404039,
    1.1218706117982236,  1.2821906010723363, 1.1169569264399697,  1.2776029202955845,
    1.167901496854562,   1.3274795256648513, 1.1343728569630767,  1.288694441290068,
    1.18912732863769,    1.3441597670003853, 1.1439916231995126,  1.2919983031375539,
    0.52555586674826027, 1.1846910126971426, 0.52555586674826027, 1.1846910126971426,
    0.60121118469659351, 1.0917758213141477, 0.59559979339073887, 1.0816344095885386,
    0.70721724131423935, 1.1827045841603281, 0.66856607747089558, 1.1103209349514938,
    0.68067133175700456, 1.0160641533162935, 0.64621319261920074, 0.95159965483556319,
    1.0742175886571268,  1.3259867989404039, 1.0742175886571268,  1.3259867989404039,
    1.1204659834462334,  1.2818083733243943, 1.1122975506136983,  1.2729804357948975,
    1.165610675352063,   1.3263908802069866, 1.1213836816613811,  1.274251635826033,
    1.1841682612752609,  1.340755258261767,  1.1284857227538327,  1.2758814346122178,
    0.52547936900151548, 1.1846910126971426, 0.52547936900151548, 1.1846910126971426,
    0.6003531114505577,  1.091082893147179,  0.59034973206035757, 1.0721439857167891,
    0.7053780301560999,  1.180348428699465,  0.65759680988423463, 1.0900868907352128,
    0.67768036558722233, 1.0116269497428043, 0.6386354775097044,  0.93782097297449063,
    1.0742175886571268,  1.3259867989404039, 1.0742175886571268,  1.3259867989404039,
    1.1204659834462334,  1.2818083733243943, 1.1122975506136983,  1.2729804357948975,
    1.165610675352063,   1.3263908802069866, 1.1213836816613811,  1.274251635826033,
    1.1841682612752609,  1.340755258261767,  1.1284857227538327,  1.2758814346122178,
    0.52547936900151548, 1.1846910126971426, 0.52547936900151548, 1.1846910126971426,
    0.6003531114505577,  1.091082893147179,  0.59034973206035757, 1.0721439857167891,
    0.7053780301560999,  1.180348428699465,  0.65759680988423463, 1.0900868907352128,
    0.67768036558722233, 1.0116269497428043, 0.6386354775097044,  0.93782097297449063,
};

/// Checks @p op against @p golden for all configurations (relative tolerance 1e-12).
template <typename Op>
void checkGolden(const Op op, const std::vector<double>& golden) {
  const std::vector<Config> all = configs({"equi", "cc", "cheb1"});
  BOOST_REQUIRE_EQUAL(golden.size(), 2 * all.size());
  size_t idx = 0;

  for (const Config& config : all) {
    LvlMIVec miVec(config.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);

    for (const DataVector& point : testPoints(genInstr->getDomain())) {
      const double result = op(SourceFunc(testFunc), point, sg);
      BOOST_TEST_CONTEXT(config.nodes << " b" << config.boundary << " unit " << config.unitDomain
                                      << " d" << config.nDim << " mivec " << config.mivec
                                      << " point " << idx % 2) {
        BOOST_CHECK_SMALL(result - golden[idx], 1e-12 * (1.0 + std::abs(golden[idx])));
      }
      idx++;
    }
  }
}

}  // namespace

BOOST_AUTO_TEST_SUITE(interpolation_pinning)

BOOST_AUTO_TEST_CASE(Global_Interpolation_Golden_Values) {
  checkGolden([](const SourceFunc& f, const DataVector& p,
                 const SparseGrid& sg) { return interpolate(f, p, sg); },
              GLOBAL_GOLDEN);
}

BOOST_AUTO_TEST_CASE(Linear_Interpolation_Golden_Values) {
  checkGolden([](const SourceFunc& f, const DataVector& p,
                 const SparseGrid& sg) { return interpolateLinear(f, p, sg); },
              LINEAR_GOLDEN);
}

BOOST_AUTO_TEST_SUITE_END()
