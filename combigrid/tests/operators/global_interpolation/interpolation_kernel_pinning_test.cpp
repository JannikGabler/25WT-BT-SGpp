// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file interpolation_kernel_pinning_test.cpp
 * @brief Boost.Test cases pinning the per-tensor-grid kernel of the global interpolation
 * (@c interpolate on a @c SparseGrid) in higher dimensions: golden values recorded with the
 * nested-collapse kernel (slices interpolated with the 1D barycentric formulas, dimension by
 * dimension) for d = 5, 6, 7, 10, equi / cc / cheb1 nodes, boundary level offsets 0 / 1, unit and
 * shifted domains, complete and anisotropic multi-index grids, three query points (generic, the
 * first coordinate on a node, the first coordinate on the lower boundary), scalar and
 * vector-valued source functions; and the evaluated points (every grid point of every tensor
 * grid once, mapped onto the domain with @c tools::mapToDomain).
 *
 * The results are pinned up to round-off: an exact reformulation of the kernel (e.g. another
 * order of the operations) changes them by a few units of the sum of the absolute terms
 * @f$\sum_\ell |c_\ell| \sum_p |L_{\ell,p}(x) f(p)|@f$ (computed here with the explicit Lagrange
 * basis), any other change by orders of magnitude more.
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <span>
#include <string>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

namespace {

/// Number of outputs of the vector-valued test function.
constexpr size_t N_OUTPUTS = 3;
/// Allowed distance to the golden values, in units of eps times the scale (see the file docs).
constexpr double ULP_TOL = 64.0;
constexpr double EPS = std::numeric_limits<double>::epsilon();

/// Output k: exp(-|x - c_k|^2 / 4) * (1 + 0.5 k x_0) - 0.5 + 0.25 k with c_k = 0.2 + 0.15 k (not
/// separable; output 0 changes its sign on the test domains).
double component(const DataVector& x, const size_t k) {
  const double center = 0.2 + 0.15 * static_cast<double>(k);
  double sqDist = 0.0;
  for (const double v : x) sqDist += (v - center) * (v - center);
  const double kd = static_cast<double>(k);
  return std::exp(-0.25 * sqDist) * (1.0 + 0.5 * kd * x[0]) - 0.5 + 0.25 * kd;
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

/// A grid of a test configuration: a complete grid of maximum level @c maxLvl, or (mivec) the
/// anisotropicMIs.
struct GridConfig {
  LvlType boundary;
  size_t nDim;
  bool mivec;
  LvlType maxLvl;
};

/// The grids per node family and domain (small enough for the debug build).
const std::vector<GridConfig> GRIDS = {
    {0, 5, false, 2}, {0, 7, false, 1},  {0, 5, true, 0}, {1, 5, false, 3},
    {1, 7, false, 2}, {1, 10, false, 2}, {1, 6, true, 0},
};

/// One test configuration.
struct Config {
  std::string nodes;
  bool unitDomain;
  GridConfig grid;
};

std::vector<Config> configs() {
  std::vector<Config> result;
  for (const std::string nodes : {"equi", "cc", "cheb1"}) {
    for (const bool unitDomain : {true, false}) {
      for (const GridConfig& grid : GRIDS) {
        result.push_back({nodes, unitDomain, grid});
      }
    }
  }
  return result;
}

/// The generation instruction of @p config (@p miVec: storage of the MIs, must outlive it).
std::unique_ptr<SGGenInstr> makeGenInstr(const Config& config, LvlMIVec& miVec) {
  const size_t nDim = config.grid.nDim;
  std::unique_ptr<SGGenInstr> genInstr;
  if (config.grid.mivec) {
    miVec = anisotropicMIs(nDim);
    genInstr = std::make_unique<MIVecSGGenInstr>(miVec);
  } else {
    genInstr = std::make_unique<CompleteSGGenInstr>(config.grid.maxLvl, nDim);
  }
  genInstr->setNodeGenFunc(nodeGenFuncByName(config.nodes));
  genInstr->setLvl2GPCntFunc(doublingLvl2GPCntFunction);
  genInstr->setBoundaryLevelOffset(config.grid.boundary);
  genInstr->setDomain(config.unitDomain ? HyperCubeArea(nDim, {0.0, 1.0}) : shiftedDomain(nDim));
  return genInstr;
}

/// Three query points: a generic one, one whose first coordinate is the domain's midpoint (a
/// node of every tensor grid) and one whose first coordinate is the lower bound of the domain.
std::vector<DataVector> testPoints(const HyperCubeArea& domain) {
  const size_t nDim = domain.size();
  DataVector generic(nDim);
  for (size_t i = 0; i < nDim; i++) {
    const double t = 0.137 + 0.211 * static_cast<double>(i);
    generic[i] = domain[i].first + (t - std::floor(t)) * (domain[i].second - domain[i].first);
  }
  DataVector onNode = generic, onBoundary = generic;
  onNode[0] = 0.5 * (domain[0].first + domain[0].second);
  onBoundary[0] = domain[0].first;
  return {generic, onNode, onBoundary};
}

/// Lagrange basis function @p j of @p nodes (n of them, starting at @p nodes) at @p x.
double lagrangeBasis(const double* const nodes, const size_t n, const size_t j, const double x) {
  double result = 1.0;
  for (size_t i = 0; i < n; i++) {
    if (i != j) result *= (x - nodes[i]) / (nodes[j] - nodes[i]);
  }
  return result;
}

/// Scale of the global interpolation of output @p k at @p point (see the file docs).
double scale(const SparseGrid& sg, const DataVector& point, const size_t k) {
  const HyperCubeArea& domain = sg.getGenInstr().getDomain();
  const DataVector normPoint = tools::normalizeDataVector(point, domain);
  double result = 0.0;

  for (const TensorGridCTData& tgData : sg) {
    const TensorGrid& tg = tgData.tensorGrid;
    const GPMI& nGPPerDim = tg.getGPCntPerDim();
    const DataVector& nodesPerDim = tg.getNodesPerDim();
    DataVector gp(tg.nDim());
    double tgSum = 0.0;

    for (size_t idx = 0; idx < tg.nGP(); idx++) {
      double basis = 1.0;
      size_t rest = idx, offset = 0;
      for (size_t dim = 0; dim < tg.nDim(); dim++) {
        const size_t n = nGPPerDim[dim];
        basis *= lagrangeBasis(nodesPerDim.data() + offset, n, rest % n, normPoint[dim]);
        rest /= n;
        offset += n;
      }
      tg.getGridPoint(idx, gp);
      tools::denormalizeDataVector(gp, domain);
      tgSum += std::abs(basis * component(gp, k));
    }
    result += std::abs(static_cast<double>(tgData.coefficient)) * tgSum;
  }

  return result;
}

/**
 * Golden values of interpolate, output k of the test function, in the order configs() x
 * testPoints() x k = 0, 1, 2 (recorded with the nested-collapse kernel, 17 significant digits).
 */
const std::vector<double> GOLDEN = {
    0.2615467152455343,    0.65515610623114062,   1.0128210393342147,    0.24534246908087259,
    0.81498110750875696,   1.3809250612973694,    0.2547177232093828,    0.58094930422096347,
    0.86483936089949842,   0.25373071392983509,   0.64885036354956949,   0.98666015472352231,
    0.23768494299208837,   0.80760503848567311,   1.345387695127938,     0.24696387729977021,
    0.57519414163694693,   0.84258318356780015,   0.2615435551993483,    0.65515430666155172,
    1.0128213561756307,    0.24533937365104216,   0.81497899994840328,   1.3809254918601057,
    0.25471458884383891,   0.58094765980135898,   0.8648396305503383,    0.26154623957238377,
    0.65515682664403485,   1.012817964944881,     0.24534346036144594,   0.81498121563512904,
    1.3809250532210631,    0.25471781980689534,   0.58095061536747217,   0.86483536632899138,
    0.25353459201252027,   0.64937666862359567,   0.98828679748368842,   0.23749718397659603,
    0.80758923820229578,   1.3454675215325356,    0.24677661428906728,   0.57591173248023919,
    0.84468250547615709,   0.15015540294753049,   0.56756113366276528,   0.91391264132195094,
    0.13649593243997926,   0.70892129551705674,   1.2384145023056599,    0.14439989772610295,
    0.50191644943141478,   0.78343485891573295,   0.26152218158464746,   0.64952176390932392,
    0.98909191649180017,   0.24531966353351195,   0.80834683818913433,   1.3485582842474602,
    0.25469469401602263,   0.57577231612371849,   0.84456952334732227,   -0.11969195848585268,
    0.12928158157606962,   0.34690464623036377,   -0.095970682209373159, 0.30372425240855716,
    0.70851711339031009,   -0.1423294024998954,   0.058918261307361747,  0.22419326753737445,
    -0.17192783925416866,  0.096768821609710509,  0.32862108320456285,   -0.15146512896104741,
    0.25639528471182838,   0.67168252065514944,   -0.19145636198924307,  0.032513814785921996,
    0.21253784306330864,   -0.11971154644770607,  0.12927002675658594,   0.34690264345021798,
    -0.09599133443484234,  0.30371010045461,      0.70851800825504763,   -0.14234768506875312,
    0.058910366047241786,  0.22419355069588404,   -0.11971792989464652,  0.12889322645461498,
    0.34620768082869136,   -0.095964615618486071, 0.30371562923181661,   0.70850958747770632,
    -0.14238313841017686,  0.058420362553417648,  0.22332544564941648,   -0.17899536387478987,
    0.087201053231902304,  0.32585506374814877,   -0.15694557840678197,  0.25202173658675209,
    0.66917679257802209,   -0.20004125780020054,  0.020613136406447463,  0.20913047572887811,
    -0.45138085548708062,  -0.18855708460135612,  0.083502091404177969,  -0.44459233231640027,
    -0.13031415002864749,  0.21888435906774695,   -0.45786028502367304,  -0.21210422941163226,
    0.037393217729154049,  -0.12327096334306242,  0.12890412774023427,   0.34562214833347943,
    -0.099758214598907421, 0.30315951091766447,   0.70578051161041089,   -0.14571598178999742,
    0.058461607487633301,  0.22312678626547647,   0.26154675485859979,   0.65515602532798445,
    1.0128209429858339,    0.24534248095439859,   0.81498110931649625,   1.3809250591221609,
    0.25471773523226049,   0.5809493056314472,    0.86483935953721858,   0.25372203640260133,
    0.64888264180837307,   0.98674257186750258,   0.23768339946815573,   0.80760707210965099,
    1.3453964795005202,    0.24696231436069646,   0.57519572836784527,   0.84258868500403516,
    0.26155220941497809,   0.65516076306637938,   1.0128243846184068,    0.24534782575946329,
    0.81498665858485397,   1.380929708019996,     0.25472314726669582,   0.58095363543659351,
    0.86484227102745548,   0.26154610640539466,   0.65515703447249596,   1.012818183234125,
    0.2453433910764854,    0.81498120211798142,   1.3809250561423836,    0.25471774073529196,
    0.58095075563073095,   0.86483549173681862,   0.2535375763098664,    0.64937262114125938,
    0.98827440998598215,   0.23749911795622869,   0.80758969267987402,   1.3454667037473578,
    0.24677850681175983,   0.57591182669648688,   0.84467810931662379,   0.15015708618392409,
    0.56754982077946448,   0.91388996605523154,   0.13649570787616994,   0.70891927887773409,
    1.2384119374506284,    0.14439982934029283,   0.50191094712289663,   0.78342642491336179,
    0.26153613403297393,   0.64952716668275357,   0.98908598260246694,   0.24533231543851447,
    0.80835734506122048,   1.3485631003107594,    0.25470738330547799,   0.57578169492843767,
    0.84457123287463287,   -0.11969575908692062,  0.12926892328879436,   0.34688668909726961,
    -0.095974101341138651, 0.30372053761075302,   0.70851385195345862,   -0.14233242931712714,
    0.05891618885184971,   0.22419223553381235,   -0.1721008715534619,   0.096734337622283117,
    0.32879309257969269,   -0.15165320545152239,  0.2561270568821552,    0.67136331411800743,
    -0.19162285839869103,  0.032364172659147913,  0.2124368378014336,    -0.11952366895673774,
    0.12941620219524272,   0.3469963157972023,    -0.095791445102830552, 0.3039322328882702,
    0.70872941440766524,   -0.14217073123479498,  0.059034291927113784,  0.22426044510850587,
    -0.11970670165317335,  0.12896502157177814,   0.34630550939340621,   -0.095963212748204324,
    0.30372028595101241,   0.70851484541540577,   -0.14236394470591565,  0.05849378109620007,
    0.22341881771771765,   -0.1786596831062397,   0.087630941246031657,  0.32628115690694415,
    -0.1566345734026435,   0.25235319028908298,   0.66947068963702705,   -0.19967776171601867,
    0.021191387025080832,  0.2098202472817241,    -0.45115031818043039,  -0.18835309164387898,
    0.083446651278460404,  -0.4443554335388944,   -0.12999137925661658,  0.21920793440287167,
    -0.45763452425598139,  -0.21189447399310768,  0.037368516345192671,  -0.12271318926024934,
    0.12939363673077556,   0.34596311409290154,   -0.099196801681572591, 0.30375238160735202,
    0.7063065669431331,    -0.1451546287057445,   0.059063167072713196,  0.22367662270264996,
    0.26154679354495713,   0.65515594497351293,   1.0128208466439181,    0.24534249226403804,
    0.81498111079761504,   1.3809250568920346,    0.2547177466841567,    0.58094930678709078,
    0.86483935814054447,   0.25371346424665547,   0.64891461048424526,   0.9868241583797559,
    0.23768188359543166,   0.80760910242562622,   1.3454051955136146,    0.24696077942064093,
    0.57519731251768158,   0.84259414362840557,   0.26156078013140149,   0.65516716001224518,
    1.0128273901325129,    0.24535619654162763,   0.81499424598400905,   1.3809338912594151,
    0.25473162334033789,   0.58095955548907596,   0.86484489088729233,   0.26154597499784549,
    0.65515723987702468,   1.012818398984292,     0.24534332268829506,   0.81498118879453885,
    1.3809250590287014,    0.25471766264223117,   0.58095089479458717,   0.86483561618329396,
    0.25354052748113454,   0.6493686082729917,    0.98826214088596487,   0.23750102974869602,
    0.80759013994574569,   1.3454658923100782,    0.24678037774025907,   0.57591191309904488,
    0.84467374735169543,   0.15015874743981394,   0.56753861277819118,   0.91386750707493736,
    0.13649548152274349,   0.70891727748064692,   1.2384093924674175,    0.14439975782559578,
    0.50190548915032895,   0.78341805645618301,   0.26154994899355999,   0.64953251935534217,
    0.98908011714202104,   0.24534484375984578,   0.8083677525408286,    1.348567878955278,
    0.25471994876708592,   0.57579098235371107,   0.84457292914474136,   -0.11969948914891071,
    0.12925674211482246,   0.34686944143544518,   -0.09597746869842283,  0.30371685875837118,
    0.70851059229993774,   -0.1423354103005332,   0.058914136449997045,  0.22419120409454274,
    -0.17226576736487689,  0.09670566533574558,   0.32896625445552896,   -0.15183260402665161,
    0.25587206813406127,   0.67106083607418865,   -0.19178167260531348,  0.03222191650841668,
    0.21234112587052809,   -0.11934167106878189,  0.12955795586472912,   0.3470873971230416,
    -0.095597815034784248, 0.30414741868897599,   0.70893442541754048,   -0.14199931847071423,
    0.059154342336903337,  0.22432531593140664,   -0.11969588698656761,  0.12903433730712244,
    0.34640007672395523,   -0.095961922212068312, 0.30372471579412597,   0.70851987233390723,
    -0.14234540945272742,  0.058564885647612086,  0.22350939727647345,   -0.17833754239053343,
    0.088039864159986103,  0.32668196430365182,   -0.15633598637551968,  0.25267034367936392,
    0.66975090659118619,   -0.19932889936212406,  0.021744010130577696,  0.21047672195268641,
    -0.45092886656867037,  -0.18816004310913192,  0.083387270189163587,  -0.44412774235771502,
    -0.12968302841484214,  0.21951510770252458,   -0.45741775095582149,  -0.21169567869781425,
    0.037339912333461678,  -0.12217526004233958,  0.12986324091994536,   0.34628722145836899,
    -0.098655184458540185, 0.30432386410564977,   0.70681362551512139,   -0.14461324117577212,
    0.059641949035292428,  0.2242043094072784,
};

/// Runs @p check(config, sg, point, golden values of the N_OUTPUTS outputs) for all
/// configurations.
template <typename Check>
void forAllConfigs(const Check check) {
  const std::vector<Config> all = configs();
  BOOST_REQUIRE_EQUAL(GOLDEN.size(), 3 * N_OUTPUTS * all.size());
  size_t idx = 0;

  for (const Config& config : all) {
    LvlMIVec miVec(config.grid.nDim, 0);
    const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
    const SparseGrid sg(*genInstr);

    for (const DataVector& point : testPoints(genInstr->getDomain())) {
      BOOST_TEST_CONTEXT(config.nodes << " b" << config.grid.boundary << " unit "
                                      << config.unitDomain << " d" << config.grid.nDim << " mivec "
                                      << config.grid.mivec << " L" << config.grid.maxLvl
                                      << " point " << point.toString()) {
        check(sg, point, std::span<const double>(GOLDEN.data() + idx, N_OUTPUTS));
      }
      idx += N_OUTPUTS;
    }
  }
}

/// Checks @p result against @p golden (output @p k) within ULP_TOL units of the scale.
void checkResult(const SparseGrid& sg, const DataVector& point, const size_t k, const double result,
                 const double golden) {
  const double tol = ULP_TOL * EPS * scale(sg, point, k) + 1e-300;
  BOOST_TEST_CONTEXT("output " << k) { BOOST_CHECK_SMALL(result - golden, tol); }
}

/// The grid points of all tensor grids of @p sg, mapped onto the domain (in no particular order).
std::vector<DataVector> allGridPoints(const SparseGrid& sg) {
  const HyperCubeArea& domain = sg.getGenInstr().getDomain();
  std::vector<DataVector> result;
  for (const TensorGridCTData& tgData : sg) {
    const TensorGrid& tg = tgData.tensorGrid;
    const GPMI& nGPPerDim = tg.getGPCntPerDim();
    const DataVector& nodesPerDim = tg.getNodesPerDim();
    for (size_t idx = 0; idx < tg.nGP(); idx++) {
      DataVector gp(tg.nDim());
      size_t rest = idx, offset = 0;
      for (size_t dim = 0; dim < tg.nDim(); dim++) {
        const double width = domain[dim].second - domain[dim].first;
        gp[dim] = tools::mapToDomain(nodesPerDim[offset + rest % nGPPerDim[dim]], domain[dim].first,
                                     width);
        rest /= nGPPerDim[dim];
        offset += nGPPerDim[dim];
      }
      result.push_back(gp);
    }
  }
  return result;
}

/// Lexicographic order of points (for comparing multisets of points bitwise).
bool lessPoint(const DataVector& a, const DataVector& b) {
  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

}  // namespace

BOOST_AUTO_TEST_SUITE(interpolation_kernel_pinning)

BOOST_AUTO_TEST_CASE(Scalar_Golden_Values) {
  forAllConfigs(
      [](const SparseGrid& sg, const DataVector& point, const std::span<const double> golden) {
        for (size_t k = 0; k < N_OUTPUTS; k++) {
          const SourceFunc f([k](const DataVector& x) { return component(x, k); });
          checkResult(sg, point, k, interpolate(f, point, sg), golden[k]);
        }
      });
}

BOOST_AUTO_TEST_CASE(Vector_Golden_Values) {
  const VectorSourceFunc f(
      [](const DataVector& x, const std::span<double> out) {
        for (size_t k = 0; k < out.size(); k++) out[k] = component(x, k);
      },
      N_OUTPUTS);
  forAllConfigs(
      [&f](const SparseGrid& sg, const DataVector& point, const std::span<const double> golden) {
        const DataVector result = interpolate(f, point, sg);
        BOOST_REQUIRE_EQUAL(result.size(), N_OUTPUTS);
        for (size_t k = 0; k < N_OUTPUTS; k++) {
          checkResult(sg, point, k, result[k], golden[k]);
        }
      });
}

BOOST_AUTO_TEST_CASE(Every_Grid_Point_Of_Every_Tensor_Grid_Evaluated_Once) {
  for (const std::string nodes : {"equi", "cc", "cheb1"}) {
    for (const GridConfig& grid : {GRIDS[0], GRIDS[4], GRIDS[6]}) {
      const Config config{nodes, false, grid};
      LvlMIVec miVec(grid.nDim, 0);
      const std::unique_ptr<SGGenInstr> genInstr = makeGenInstr(config, miVec);
      const SparseGrid sg(*genInstr);
      std::vector<DataVector> expected = allGridPoints(sg);
      std::ranges::sort(expected, lessPoint);

      for (const DataVector& point : testPoints(genInstr->getDomain())) {
        std::vector<DataVector> evaluated;
        const SourceFunc f([&evaluated](const DataVector& x) {
#pragma omp critical
          evaluated.push_back(x);
          return component(x, 0);
        });
        interpolate(f, point, sg);
        std::ranges::sort(evaluated, lessPoint);
        BOOST_TEST_CONTEXT(nodes << " b" << grid.boundary << " d" << grid.nDim << " point "
                                 << point.toString()) {
          BOOST_REQUIRE_EQUAL(evaluated.size(), expected.size());
          BOOST_CHECK(evaluated == expected);
        }
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
