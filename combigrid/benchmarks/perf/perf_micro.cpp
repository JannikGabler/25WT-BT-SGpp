// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_micro.cpp
 * @brief Micro-benchmarks of the combigrid performance suite: single internal functions at the
 * hot spots (one tensor grid, one 1D rule, one multi-index set operation, ...).
 *
 * NOT protected: work packages add cases for the spot they optimize and may adapt existing cases
 * when an internal signature changes (the summary must say so). Existing case names keep their
 * meaning; a case whose meaning changes gets a new name.
 *
 * Runs single-threaded by default in perf.py (--threads 1). Each case reports the time per call
 * and, via MicroRun::setItems, the time per item (grid point, node, multi-index, ...).
 *
 * Case names: micro/<area>/<function>/<configuration>. Cases named micro/sol/... are
 * "speed of light" references: minimal hand-written loops for the same task, used to estimate
 * the remaining potential of the spot.
 */

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/generic_dsg_operator_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/global_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/global_interpolation/interpolation_methods/default_methods/barycentric_formula.hpp>
#include <sgpp/combigrid/tools/operators/linear_interpolation/linear_interpolation_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

#include "perf_cache.hpp"
#include "perf_cache_proto.hpp"
#include "perf_harness.hpp"
#include "perf_interp_sol.hpp"
#include "perf_problems.hpp"

using sgpp::combigrid::CompleteSGGenInstr;
using sgpp::combigrid::DeltasSparseGrid;
using sgpp::combigrid::GPCntType;
using sgpp::combigrid::HyperCubeArea;
using sgpp::combigrid::LvlMIVec;
using sgpp::combigrid::LvlType;
using sgpp::combigrid::SourceFunc;
using sgpp::combigrid::SparseGrid;
using sgpp::combigrid::TensorGrid;
using sgpp::combigrid::TensorGridCTData;
using sgpp::combigrid::perf::DataVector;
using sgpp::combigrid::perf::doNotOptimize;
using sgpp::combigrid::perf::MicroRun;
using sgpp::combigrid::perf::Suite;

namespace cg = sgpp::combigrid;
namespace perf = sgpp::combigrid::perf;
namespace sol = sgpp::combigrid::perf::interp_sol;

namespace {

std::string cfg(const std::string& nodes, const LvlType boundary, const size_t nDim,
                const LvlType maxLvl) {
  return nodes + "/b" + std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" +
         std::to_string(maxLvl);
}

/// Tensor grids processed by a quadrature case: the largest one, or all (allTGs).
template <typename Grid>
std::vector<const TensorGridCTData*> selectTGs(const Grid& grid, const bool allTGs) {
  std::vector<const TensorGridCTData*> tgs;
  const TensorGridCTData* largest = &*grid.begin();
  for (const TensorGridCTData& tgData : grid) {
    if (allTGs) tgs.push_back(&tgData);
    if (tgData.tensorGrid.nGP() > largest->tensorGrid.nGP()) largest = &tgData;
  }
  if (!allTGs) tgs.push_back(largest);
  return tgs;
}

/// Output k of the vector integrands: exp(1/2 sum_i s_k x_i) with s_k = 1 + k/64 (output 0 is
/// perf::expSum). The factor inside the sum keeps the compiler from sharing work between outputs:
/// an m-output evaluation costs m scalar evaluations.
double expSumK(const DataVector& x, const size_t k) {
  const double scale = 1.0 + static_cast<double>(k) / 64.0;
  double sum = 0.0;
  for (size_t i = 0; i < x.size(); i++) sum += x[i] * scale;
  return std::exp(0.5 * sum);
}

/// Output k of the vector interpolation targets: perf::cosProd shifted by 0.01 k.
double cosProdK(const DataVector& x, const size_t k) {
  const double shift = 0.01 * static_cast<double>(k);
  double result = 1.0;
  for (size_t i = 0; i < x.size(); i++) {
    result *= std::cos(1.5 * x[i] + 0.2 * static_cast<double>(i) + shift);
  }
  return result;
}

/// Sets the counters bitwise_equal (1 if vec equals scalars bitwise) and max_ulp (largest
/// distance in units in the last place) of a case; reports distances above 16 ulp on stderr.
void compareResults(MicroRun& run, const std::string& name, const std::vector<double>& vec,
                    const std::vector<double>& scalars) {
  double maxUlp = 0.0;
  for (size_t k = 0; k < vec.size(); k++) {
    const int64_t a = std::bit_cast<int64_t>(vec[k]);
    const int64_t b = std::bit_cast<int64_t>(scalars[k]);
    maxUlp = std::max(maxUlp, std::abs(static_cast<double>(a - b)));
  }
  if (maxUlp > 16.0) std::fprintf(stderr, "WARN %s: %.0f ulp\n", name.c_str(), maxUlp);
  run.setCounter("bitwise_equal", (maxUlp == 0.0) ? 1.0 : 0.0);
  run.setCounter("max_ulp", maxUlp);
}

template <typename Grid>
const TensorGridCTData& largestTensorGrid(const Grid& grid) {
  const TensorGridCTData* largest = &*grid.begin();
  for (const TensorGridCTData& tgData : grid) {
    if (tgData.tensorGrid.nGP() > largest->tensorGrid.nGP()) largest = &tgData;
  }
  return *largest;
}

/// Number of grid points of all tensor grids of @p grid.
template <typename Grid>
double sumOfGridPoints(const Grid& grid) {
  double sum = 0.0;
  for (const TensorGridCTData& tgData : grid) sum += static_cast<double>(tgData.tensorGrid.nGP());
  return sum;
}

/*******************************************************************************
Complete sparse grid quadrature
*******************************************************************************/

/// tools::quadrature::processTensorGrid on the largest tensor grid (perf::expSum).
void addProcessTG(Suite& suite, const std::string& nodes, const LvlType boundary, const size_t nDim,
                  const LvlType maxLvl) {
  suite.addMicro("micro/quad/process_tg/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::expSum);
    const cg::misc::QuadratureContext context{.sparseGrid = sg,
                                              .domain = sg.getGenInstr().getDomain(),
                                              .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
    cg::misc::QuadratureScratch scratch(nDim, sg.getMaxTGSumOverGPCntsPerDim());
    const TensorGrid& tg = largestTensorGrid(sg).tensorGrid;

    run.setItems(static_cast<double>(tg.nGP()));
    run.measure([&]() {
      doNotOptimize(cg::tools::quadrature::processTensorGrid(context, sourceFunc, tg, scratch));
    });
  });
}

/// tools::quadrature::processTensorGrid over all tensor grids, serially (regime of many
/// small tensor grids, where per-tensor-grid costs matter).
void addProcessAllTGs(Suite& suite, const std::string& nodes, const LvlType boundary,
                      const size_t nDim, const LvlType maxLvl) {
  suite.addMicro(
      "micro/quad/process_all_tgs/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
        const SparseGrid sg(genInstr);
        const SourceFunc sourceFunc(perf::expSum);
        const cg::misc::QuadratureContext context{
            .sparseGrid = sg,
            .domain = sg.getGenInstr().getDomain(),
            .nodeGenFuncs = sg.getGenInstr().getNodeGenFuncs()};
        cg::misc::QuadratureScratch scratch(nDim, sg.getMaxTGSumOverGPCntsPerDim());
        size_t nGP = 0;
        for (const TensorGridCTData& tgData : sg) nGP += tgData.tensorGrid.nGP();

        run.setItems(static_cast<double>(nGP));
        run.setCounter("nTG", static_cast<double>(sg.nTG()));
        run.measure([&]() {
          double sum = 0.0;
          for (const TensorGridCTData& tgData : sg) {
            sum += tgData.coefficient * cg::tools::quadrature::processTensorGrid(
                                            context, sourceFunc, tgData.tensorGrid, scratch);
          }
          doNotOptimize(sum);
        });
      });
}

/// Tensor grids and context of the vector-valued quadrature cases (addVectorQuad).
struct VectorQuadSetup {
  VectorQuadSetup(const std::string& nodes, const LvlType boundary, const size_t nDim,
                  const LvlType maxLvl, const bool allTGs)
      : genInstr(maxLvl, nDim) {
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    sg.emplace(genInstr);
    tgs = selectTGs(*sg, allTGs);
    for (const TensorGridCTData* tgData : tgs) nGP += static_cast<double>(tgData->tensorGrid.nGP());
  }

  cg::misc::QuadratureContext context() const {
    return {.sparseGrid = *sg,
            .domain = sg->getGenInstr().getDomain(),
            .nodeGenFuncs = sg->getGenInstr().getNodeGenFuncs()};
  }

  CompleteSGGenInstr genInstr;
  std::optional<SparseGrid> sg;
  std::vector<const TensorGridCTData*> tgs;  ///< The largest tensor grid or all of them.
  double nGP = 0.0;
};

/**
 * WP-29a: SG quadrature of a vector-valued source function with the production kernels, ns per
 * grid point and output: micro/quad/<kernel>_vec/<cfg>/m<m> (one VectorSourceFunc with the m
 * outputs expSumK) vs. micro/quad/<kernel>_scalar_x_m/<cfg>/m<m> (m scalar runs, one
 * SourceFunc per output); <kernel> = process_tg (largest tensor grid) or process_all_tgs (all,
 * serially, with the combination). Counter max_ulp: vector vs. scalar results per tensor grid.
 */
void addVectorQuad(Suite& suite, const std::string& nodes, const LvlType boundary,
                   const size_t nDim, const LvlType maxLvl, const bool allTGs, const size_t m) {
  const std::string name = cfg(nodes, boundary, nDim, maxLvl) + "/m" + std::to_string(m);
  const std::string kernel = allTGs ? "micro/quad/process_all_tgs" : "micro/quad/process_tg";

  suite.addMicro(kernel + "_scalar_x_m/" + name, [=](MicroRun& run) {
    const VectorQuadSetup s(nodes, boundary, nDim, maxLvl, allTGs);
    const cg::misc::QuadratureContext context = s.context();
    cg::misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
    std::vector<SourceFunc> funcs;
    for (size_t k = 0; k < m; k++) {
      funcs.emplace_back([k](const DataVector& x) { return expSumK(x, k); });
    }

    run.setItems(s.nGP * static_cast<double>(m));
    run.measure([&]() {
      for (const SourceFunc& f : funcs) {
        double sum = 0.0;
        for (const TensorGridCTData* tgData : s.tgs) {
          sum += tgData->coefficient *
                 cg::tools::quadrature::processTensorGrid(context, f, tgData->tensorGrid, scratch);
        }
        doNotOptimize(sum);
      }
    });
  });

  suite.addMicro(kernel + "_vec/" + name, [=](MicroRun& run) {
    const VectorQuadSetup s(nodes, boundary, nDim, maxLvl, allTGs);
    const cg::misc::QuadratureContext context = s.context();
    cg::misc::QuadratureScratch scratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim(), m);
    const cg::VectorSourceFunc f(
        [](const DataVector& x, std::span<double> out) {
          for (size_t k = 0; k < out.size(); k++) out[k] = expSumK(x, k);
        },
        m);
    std::vector<double> tgResult(m), sum(m);

    // Vector vs. scalar results tensor grid by tensor grid (tensor grid-major, output-minor)
    std::vector<double> vecPerTG, scalarPerTG;
    for (const TensorGridCTData* tgData : s.tgs) {
      cg::tools::quadrature::processTensorGrid(context, f, tgData->tensorGrid, scratch, tgResult);
      vecPerTG.insert(vecPerTG.end(), tgResult.begin(), tgResult.end());
      cg::misc::QuadratureScratch scalarScratch(nDim, s.sg->getMaxTGSumOverGPCntsPerDim());
      for (size_t k = 0; k < m; k++) {
        const SourceFunc fk([k](const DataVector& x) { return expSumK(x, k); });
        scalarPerTG.push_back(cg::tools::quadrature::processTensorGrid(
            context, fk, tgData->tensorGrid, scalarScratch));
      }
    }
    compareResults(run, name, vecPerTG, scalarPerTG);

    run.setItems(s.nGP * static_cast<double>(m));
    run.measure([&]() {
      std::fill(sum.begin(), sum.end(), 0.0);
      for (const TensorGridCTData* tgData : s.tgs) {
        cg::tools::quadrature::processTensorGrid(context, f, tgData->tensorGrid, scratch, tgResult);
        for (size_t k = 0; k < m; k++) sum[k] += tgData->coefficient * tgResult[k];
      }
      doNotOptimize(sum.data());
      perf::clobberMemory();
    });
  });
}

/**
 * Speed of light for addProcessTG: the same weighted sum over the same tensor grid with the same
 * integrand call (SourceFunc::evaluate), but with nodes mapped onto the domain once and the
 * tensor-product weight updated per row (dimension 0 fastest).
 */
void addSolQuadTG(Suite& suite, const std::string& nodes, const LvlType boundary, const size_t nDim,
                  const LvlType maxLvl) {
  suite.addMicro("micro/sol/quad_tg/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::expSum);
    const TensorGrid& tg = largestTensorGrid(sg).tensorGrid;
    const HyperCubeArea& domain = sg.getGenInstr().getDomain();
    const cg::GPMI& nPerDim = tg.getGPCntPerDim();

    // Per-dimension nodes on the domain and 1D weights
    std::vector<std::vector<double>> x(nDim), w(nDim);
    size_t offset = 0;
    for (size_t k = 0; k < nDim; k++) {
      const GPCntType n = nPerDim[k];
      x[k].resize(n);
      w[k].resize(n);
      for (GPCntType i = 0; i < n; i++) {
        x[k][i] = tg.getNodesPerDim()[offset + i] * (domain[k].second - domain[k].first) +
                  domain[k].first;
      }
      sg.getGenInstr().getNodeGenFuncForDim(k)->getQuadRule(n)->genWeightsInplace(w[k]);
      offset += n;
    }

    run.setItems(static_cast<double>(tg.nGP()));
    run.measure([&]() {
      std::vector<GPCntType> idx(nDim, 0);
      DataVector gp(nDim);
      for (size_t k = 0; k < nDim; k++) gp[k] = x[k][0];
      double sum = 0.0;
      while (true) {
        double rowWeight = 1.0;
        for (size_t k = 1; k < nDim; k++) rowWeight *= w[k][idx[k]];
        double rowSum = 0.0;
        for (GPCntType i = 0; i < nPerDim[0]; i++) {
          gp[0] = x[0][i];
          rowSum += w[0][i] * sourceFunc.evaluate(gp);
        }
        sum += rowWeight * rowSum;
        size_t k = 1;
        for (; k < nDim; k++) {
          if (++idx[k] < nPerDim[k]) {
            gp[k] = x[k][idx[k]];
            break;
          }
          idx[k] = 0;
          gp[k] = x[k][0];
        }
        if (k >= nDim) break;
      }
      doNotOptimize(sum);
    });
  });
}

/// Iterating all grid points of the largest tensor grid with the mutable range (as quadrature).
void addIterateTG(Suite& suite, const size_t nDim, const LvlType maxLvl) {
  suite.addMicro("micro/tg/iterate_mutable/" + cfg("equi", 0, nDim, maxLvl), [=](MicroRun& run) {
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, "equi", 0, perf::unitDomain(nDim));
    const SparseGrid sg(genInstr);
    const TensorGrid& tg = largestTensorGrid(sg).tensorGrid;

    run.setItems(static_cast<double>(tg.nGP()));
    run.measure([&]() {
      double sum = 0.0;
      for (auto& [mi, gp] : tg.gridPointsMutable()) sum += gp[0] + static_cast<double>(mi[0]);
      doNotOptimize(sum);
    });
  });
}

/// SourceFunc::evaluateNormalizedInPlace (affine map + std::function call), per point.
void addEvalNormalized(Suite& suite, const size_t nDim) {
  suite.addMicro(
      "micro/sourcefunc/eval_normalized_in_place/d" + std::to_string(nDim), [=](MicroRun& run) {
        const SourceFunc sourceFunc(perf::expSum);
        const HyperCubeArea domain = perf::shiftedDomain(nDim);
        const std::vector<DataVector> points = perf::randomPoints(perf::unitDomain(nDim), 1024, 7);
        DataVector gp(nDim);

        run.setItems(static_cast<double>(points.size()));
        run.measure([&]() {
          double sum = 0.0;
          for (const DataVector& point : points) {
            std::copy(point.begin(), point.end(), gp.begin());
            sum += sourceFunc.evaluateNormalizedInPlace(gp, domain);
          }
          doNotOptimize(sum);
        });
      });
}

/// 1D Clenshaw-Curtis weights (generated once per tensor grid and dimension by quadrature()).
void addCCWeights(Suite& suite, const GPCntType nNodes) {
  suite.addMicro("micro/quadrule/cc_weights/n" + std::to_string(nNodes), [=](MicroRun& run) {
    cg::QuadRule* const rule = cg::getClenshawCurtisQuadRule();
    std::vector<double> weights(nNodes);

    run.setItems(static_cast<double>(nNodes));
    run.measure([&]() {
      rule->genWeightsInplace(weights);
      doNotOptimize(weights.data());
    });
  });
}

/*******************************************************************************
Deltas sparse grid quadrature
*******************************************************************************/

/// tools::dsg_quadrature::processDeltaTensorGrid on the largest delta tensor grid.
void addProcessDeltaTG(Suite& suite, const std::string& nodes, const LvlType boundary,
                       const size_t nDim, const LvlType maxLvl) {
  suite.addMicro(
      "micro/dsg/process_delta_tg/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
        const DeltasSparseGrid dsg(genInstr);
        const SourceFunc sourceFunc(perf::expSum);
        const cg::misc::FlatNestedVec<double, 3> weights1D =
            cg::tools::dsg_quadrature::comp1DWeights(dsg);
        const cg::misc::CTCoeffTrie coeffTrie(dsg.getTensorGrids(), nDim);
        const cg::misc::GenericDSGOperatorContext context{.deltasSparseGrid = dsg,
                                                          .domain = dsg.getGenInstr().getDomain(),
                                                          .weights1D = weights1D,
                                                          .coeffTrie = coeffTrie};
        cg::misc::DeltaQuadratureScratch scratch(nDim, dsg.getMaxTGSumOverGPCntsPerDim());
        const TensorGridCTData& tgData = largestTensorGrid(dsg);

        run.setItems(static_cast<double>(tgData.tensorGrid.nGP()));
        run.measure([&]() {
          doNotOptimize(cg::tools::dsg_quadrature::processDeltaTensorGrid(context, sourceFunc,
                                                                          tgData, scratch));
        });
      });
}

/// Preparation of the deltas quadrature: 1D weights in delta order and coefficient trie.
void addDeltaPrep(Suite& suite, const std::string& nodes, const LvlType boundary, const size_t nDim,
                  const LvlType maxLvl) {
  suite.addMicro("micro/dsg/prepare/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    const DeltasSparseGrid dsg(genInstr);

    run.setItems(static_cast<double>(dsg.nTG()));
    run.measure([&]() {
      const cg::misc::FlatNestedVec<double, 3> weights1D =
          cg::tools::dsg_quadrature::comp1DWeights(dsg);
      const cg::misc::CTCoeffTrie coeffTrie(dsg.getTensorGrids(), nDim);
      doNotOptimize(weights1D.flat().data());
      doNotOptimize(coeffTrie.leafCoeffs().data());
    });
  });
}

/// tools::dsg_quadrature::compSubTrie (the per-delta-tensor-grid trie work) for every
/// @p stride -th delta tensor grid (all of them take minutes at d20).
void addSubTries(Suite& suite, const std::string& nodes, const LvlType boundary, const size_t nDim,
                 const LvlType maxLvl, const size_t stride) {
  suite.addMicro(
      "micro/dsg/sub_tries/" + cfg(nodes, boundary, nDim, maxLvl) + "/s" + std::to_string(stride),
      [=](MicroRun& run) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
        const DeltasSparseGrid dsg(genInstr);
        const cg::misc::FlatNestedVec<double, 3> weights1D =
            cg::tools::dsg_quadrature::comp1DWeights(dsg);
        const cg::misc::CTCoeffTrie coeffTrie(dsg.getTensorGrids(), nDim);
        const cg::misc::GenericDSGOperatorContext context{.deltasSparseGrid = dsg,
                                                          .domain = dsg.getGenInstr().getDomain(),
                                                          .weights1D = weights1D,
                                                          .coeffTrie = coeffTrie};
        cg::misc::DeltaQuadratureScratch scratch(nDim, dsg.getMaxTGSumOverGPCntsPerDim());

        run.setItems(static_cast<double>((dsg.nTG() + stride - 1) / stride));
        run.measure([&]() {
          size_t nNodes = 0;
          for (size_t tg = 0; tg < dsg.nTG(); tg += stride) {
            cg::tools::dsg_quadrature::compSubTrie(context, dsg.getTensorGrid(tg).mi, scratch);
            nNodes += scratch.trieNodes[0].size();
          }
          doNotOptimize(nNodes);
        });
      });
}

/// tools::dsg_quadrature::processDeltaTensorGrid (sub-trie, global weights, integrand
/// perf::expSum) for every @p stride -th delta tensor grid: the whole per-delta-tensor-grid work
/// of the operator at 1 thread, as opposed to process_delta_tg on the largest grid only.
/// @p constant: micro/dsg/row_loop/...: the same with a constant integrand, i.e. the row loop
/// (global weights, grid points, sub-tries) plus the call of the source function only.
void addProcessDeltaTGs(Suite& suite, const std::string& nodes, const LvlType boundary,
                        const size_t nDim, const LvlType maxLvl, const size_t stride,
                        const bool constant = false) {
  suite.addMicro(
      std::string(constant ? "micro/dsg/row_loop/" : "micro/dsg/process_delta_tgs/") +
          cfg(nodes, boundary, nDim, maxLvl) + "/s" + std::to_string(stride),
      [=](MicroRun& run) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
        const DeltasSparseGrid dsg(genInstr);
        const SourceFunc sourceFunc =
            constant ? SourceFunc([](const DataVector&) { return 1.0; }) : SourceFunc(perf::expSum);
        const cg::misc::FlatNestedVec<double, 3> weights1D =
            cg::tools::dsg_quadrature::comp1DWeights(dsg);
        const cg::misc::CTCoeffTrie coeffTrie(dsg.getTensorGrids(), nDim);
        const cg::misc::GenericDSGOperatorContext context{.deltasSparseGrid = dsg,
                                                          .domain = dsg.getGenInstr().getDomain(),
                                                          .weights1D = weights1D,
                                                          .coeffTrie = coeffTrie};
        cg::misc::DeltaQuadratureScratch scratch(nDim, dsg.getMaxTGSumOverGPCntsPerDim());

        size_t nGP = 0;
        for (size_t tg = 0; tg < dsg.nTG(); tg += stride) {
          nGP += dsg.getTensorGrid(tg).tensorGrid.nGP();
        }
        run.setItems(static_cast<double>(nGP));
        run.measure([&]() {
          double result = 0.0;
          for (size_t tg = 0; tg < dsg.nTG(); tg += stride) {
            result += cg::tools::dsg_quadrature::processDeltaTensorGrid(
                context, sourceFunc, dsg.getTensorGrid(tg), scratch);
          }
          doNotOptimize(result);
        });
      });
}

/// Delta tensor grids and context of the vector-valued DSG quadrature cases (addVectorDeltaQuad).
struct VectorDeltaQuadSetup {
  /// @p stride 0: the largest delta tensor grid; else every @p stride -th one.
  VectorDeltaQuadSetup(const std::string& nodes, const LvlType boundary, const size_t nDim,
                       const LvlType maxLvl, const size_t stride)
      : genInstr(maxLvl, nDim) {
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    dsg.emplace(genInstr);
    weights1D.emplace(cg::tools::dsg_quadrature::comp1DWeights(*dsg));
    coeffTrie.emplace(dsg->getTensorGrids(), nDim);
    if (stride == 0) {
      tgs.push_back(&largestTensorGrid(*dsg));
    } else {
      for (size_t tg = 0; tg < dsg->nTG(); tg += stride) tgs.push_back(&dsg->getTensorGrid(tg));
    }
    for (const TensorGridCTData* tgData : tgs) nGP += static_cast<double>(tgData->tensorGrid.nGP());
  }

  cg::misc::GenericDSGOperatorContext context() const {
    return {.deltasSparseGrid = *dsg,
            .domain = dsg->getGenInstr().getDomain(),
            .weights1D = *weights1D,
            .coeffTrie = *coeffTrie};
  }

  CompleteSGGenInstr genInstr;
  std::optional<DeltasSparseGrid> dsg;
  std::optional<cg::misc::FlatNestedVec<double, 3>> weights1D;
  std::optional<cg::misc::CTCoeffTrie> coeffTrie;
  std::vector<const TensorGridCTData*> tgs;
  double nGP = 0.0;
};

/**
 * WP-29b: DSG quadrature of a vector-valued source function with the production kernels, ns per
 * grid point and output: micro/dsg/<kernel>_vec/<cfg>/m<m> (one VectorSourceFunc with the m
 * outputs expSumK) vs. micro/dsg/<kernel>_scalar_x_m/<cfg>/m<m> (m scalar runs, one
 * SourceFunc per output); <kernel> = process_delta_tg (largest delta tensor grid, @p stride 0) or
 * process_delta_tgs (every @p stride -th one, cfg suffix /s<stride>: incl. the sub-tries, which
 * the vector run builds once for all outputs). Counter max_ulp: vector vs. scalar results per
 * delta tensor grid.
 */
void addVectorDeltaQuad(Suite& suite, const std::string& nodes, const LvlType boundary,
                        const size_t nDim, const LvlType maxLvl, const size_t stride,
                        const size_t m) {
  const std::string name = cfg(nodes, boundary, nDim, maxLvl) +
                           (stride == 0 ? "" : "/s" + std::to_string(stride)) + "/m" +
                           std::to_string(m);
  const std::string kernel =
      stride == 0 ? "micro/dsg/process_delta_tg" : "micro/dsg/process_delta_tgs";

  suite.addMicro(kernel + "_scalar_x_m/" + name, [=](MicroRun& run) {
    const VectorDeltaQuadSetup s(nodes, boundary, nDim, maxLvl, stride);
    const cg::misc::GenericDSGOperatorContext context = s.context();
    cg::misc::DeltaQuadratureScratch scratch(nDim, s.dsg->getMaxTGSumOverGPCntsPerDim());
    std::vector<SourceFunc> funcs;
    for (size_t k = 0; k < m; k++) {
      funcs.emplace_back([k](const DataVector& x) { return expSumK(x, k); });
    }

    run.setItems(s.nGP * static_cast<double>(m));
    run.measure([&]() {
      for (const SourceFunc& f : funcs) {
        double sum = 0.0;
        for (const TensorGridCTData* tgData : s.tgs) {
          sum += cg::tools::dsg_quadrature::processDeltaTensorGrid(context, f, *tgData, scratch);
        }
        doNotOptimize(sum);
      }
    });
  });

  suite.addMicro(kernel + "_vec/" + name, [=](MicroRun& run) {
    const VectorDeltaQuadSetup s(nodes, boundary, nDim, maxLvl, stride);
    const cg::misc::GenericDSGOperatorContext context = s.context();
    cg::misc::DeltaQuadratureScratch scratch(nDim, s.dsg->getMaxTGSumOverGPCntsPerDim(), m);
    const cg::VectorSourceFunc f(
        [](const DataVector& x, std::span<double> out) {
          for (size_t k = 0; k < out.size(); k++) out[k] = expSumK(x, k);
        },
        m);
    std::vector<double> tgResult(m), sum(m);

    // Vector vs. scalar results delta tensor grid by delta tensor grid (grid-major, output-minor)
    std::vector<double> vecPerTG, scalarPerTG;
    cg::misc::DeltaQuadratureScratch scalarScratch(nDim, s.dsg->getMaxTGSumOverGPCntsPerDim());
    for (const TensorGridCTData* tgData : s.tgs) {
      cg::tools::dsg_quadrature::processDeltaTensorGrid(context, f, *tgData, scratch, tgResult);
      vecPerTG.insert(vecPerTG.end(), tgResult.begin(), tgResult.end());
      for (size_t k = 0; k < m; k++) {
        const SourceFunc fk([k](const DataVector& x) { return expSumK(x, k); });
        scalarPerTG.push_back(
            cg::tools::dsg_quadrature::processDeltaTensorGrid(context, fk, *tgData, scalarScratch));
      }
    }
    compareResults(run, name, vecPerTG, scalarPerTG);

    run.setItems(s.nGP * static_cast<double>(m));
    run.measure([&]() {
      std::fill(sum.begin(), sum.end(), 0.0);
      for (const TensorGridCTData* tgData : s.tgs) {
        cg::tools::dsg_quadrature::processDeltaTensorGrid(context, f, *tgData, scratch, tgResult);
        for (size_t k = 0; k < m; k++) sum[k] += tgResult[k];
      }
      doNotOptimize(sum.data());
      perf::clobberMemory();
    });
  });
}

/*******************************************************************************
Interpolation
*******************************************************************************/

/// global_interpolation::interpolate on the largest tensor grid (perf::cosProd).
void addInterpTG(Suite& suite, const std::string& nodes, const size_t nDim, const LvlType maxLvl) {
  suite.addMicro("micro/interp/global_tg/" + cfg(nodes, 0, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, 0, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 99)[0];
    const DataVector normPoint = cg::tools::normalizeDataVector(point, domain);
    const TensorGridCTData& tgData = largestTensorGrid(sg);

    run.setItems(static_cast<double>(tgData.tensorGrid.nGP()));
    run.measure([&]() {
      doNotOptimize(cg::tools::global_interpolation::processTensorGrid(sourceFunc, normPoint,
                                                                       tgData, sg.getGenInstr()));
    });
  });
}

/**
 * The complete global interpolation operator interpolate on a SparseGrid at one query point
 * (perf::cosProd on the shifted domain, boundary level offset 0; items = grid points of all
 * tensor grids). Control case of the deltas sparse grid operator micro/interp/dsg/...
 */
void addGlobalSGOperator(Suite& suite, const std::string& nodes, const size_t nDim,
                         const LvlType maxLvl) {
  suite.addMicro("micro/interp/sg/" + cfg(nodes, 0, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, 0, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 97)[0];

    run.setItems(sumOfGridPoints(sg));
    run.measure([&]() { doNotOptimize(sgpp::combigrid::interpolate(sourceFunc, point, sg)); });
  });
}

/// One 1D barycentric interpolation (generic formula: cheb1 nodes; optimized: cc nodes).
void addBarycentric1D(Suite& suite, const bool optimized, const GPCntType nNodes) {
  suite.addMicro(
      std::string("micro/interp/barycentric_1d/") + (optimized ? "opt_cc" : "generic") + "/n" +
          std::to_string(nNodes),
      [=](MicroRun& run) {
        cg::NodeGenFunc* const nodeGenFunc = perf::nodeGenFuncByName(optimized ? "cc" : "cheb1");
        cg::InterpolationMethod* const method =
            optimized ? cg::getOptBarycentricFormula() : cg::getBarycentricFormula(nodeGenFunc);
        const DataVector nodesVec = nodeGenFunc->genNodes(nNodes, optimized);
        const std::vector<double> nodeValues(nodesVec.begin(), nodesVec.end());
        std::vector<double> values(nNodes);
        for (GPCntType i = 0; i < nNodes; i++) values[i] = std::cos(3.0 * nodeValues[i]);

        run.setItems(static_cast<double>(nNodes));
        run.measure([&]() { doNotOptimize(method->interpolate(0.3141, nodeValues, values)); });
      });
}

/// linear_interpolation::interpolate on the largest tensor grid (2^d corners).
void addLinearTG(Suite& suite, const size_t nDim, const LvlType maxLvl) {
  suite.addMicro("micro/interplin/tg/" + cfg("equi", 1, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, "equi", 1, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 98)[0];
    const DataVector normPoint = cg::tools::normalizeDataVector(point, domain);
    const TensorGridCTData& tgData = largestTensorGrid(sg);
    cg::misc::LinInterpScratch scratch(nDim);

    run.setItems(std::pow(2.0, static_cast<double>(nDim)));
    run.measure([&]() {
      doNotOptimize(cg::tools::lin_interpolation::processTensorGrid(sourceFunc, normPoint, tgData,
                                                                    sg.getGenInstr(), scratch));
    });
  });
}

/**
 * The complete linear interpolation operator interpolateLinear on a SparseGrid at one query point
 * (the micro/interplin/tg configurations; 2^d corners per tensor grid, items = corners of all
 * tensor grids). Control case of the deltas sparse grid operator micro/interplin/dsg/...
 */
void addLinearSGOperator(Suite& suite, const size_t nDim, const LvlType maxLvl) {
  suite.addMicro("micro/interplin/sg/" + cfg("equi", 1, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, "equi", 1, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 98)[0];

    run.setItems(static_cast<double>(sg.nTG()) * std::pow(2.0, static_cast<double>(nDim)));
    run.measure(
        [&]() { doNotOptimize(sgpp::combigrid::interpolateLinear(sourceFunc, point, sg)); });
  });
}

/**
 * Speed of light for addInterpTG (perf_interp_sol.hpp): the same source-function calls and 1D
 * rules on the same tensor grid and point, with the nodes mapped onto the domain once, grid
 * points assembled incrementally and scratch reused; integrandOnly: the source-function calls
 * alone (sum of the values).
 */
void addSolInterpTG(Suite& suite, const std::string& nodes, const size_t nDim, const LvlType maxLvl,
                    const bool integrandOnly) {
  const std::string kind = integrandOnly ? "interp_tg_integrand/" : "interp_tg/";
  suite.addMicro("micro/sol/" + kind + cfg(nodes, 0, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, 0, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 99)[0];
    const DataVector normPoint = cg::tools::normalizeDataVector(point, domain);
    const TensorGrid& tg = largestTensorGrid(sg).tensorGrid;
    sol::NodesPerDim unit, mapped;
    sol::tgNodes(tg, domain, unit, mapped);
    std::vector<const cg::InterpolationMethod*> methods;
    for (size_t k = 0; k < nDim; k++) {
      methods.push_back(sg.getGenInstr().getNodeGenFuncForDim(k)->getInterpolationMethod(
          static_cast<GPCntType>(unit[k].size())));
    }
    sol::GlobalScratch scratch(nDim);

    run.setItems(static_cast<double>(tg.nGP()));
    if (integrandOnly) {
      run.measure([&]() {
        double sum = 0.0;
        sol::forEachValue(sourceFunc, mapped, scratch, [&](size_t, const double v) { sum += v; });
        doNotOptimize(sum);
      });
    } else {
      run.measure([&]() {
        doNotOptimize(sol::globalKernelTG(sourceFunc, unit, mapped, sol::ProductionRule(methods),
                                          tg.nGP(), normPoint, scratch));
      });
    }
  });
}

/**
 * Speed of light for addBarycentric1D (generic formula): the weights computed once in the setup,
 * O(n) per call (the formula of BarycentricFormula without the weights and without the
 * trivial-result scan).
 */
void addSolBarycentricCached(Suite& suite, const GPCntType nNodes) {
  suite.addMicro("micro/sol/barycentric_1d/generic_cached_weights/n" + std::to_string(nNodes),
                 [=](MicroRun& run) {
                   cg::NodeGenFunc* const nodeGenFunc = perf::nodeGenFuncByName("cheb1");
                   const DataVector nodesVec = nodeGenFunc->genNodes(nNodes, false);
                   const std::vector<double> x(nodesVec.begin(), nodesVec.end());
                   const std::vector<double> w = cg::tools::barycentricFormulaWeights(x);
                   std::vector<double> values(nNodes);
                   for (GPCntType i = 0; i < nNodes; i++) values[i] = std::cos(3.0 * x[i]);

                   run.setItems(static_cast<double>(nNodes));
                   run.measure([&]() {
                     double numerator = 0.0, denominator = 0.0;
                     for (GPCntType i = 0; i < nNodes; i++) {
                       const double factor = w[i] / (0.3141 - x[i]);
                       numerator += factor * values[i];
                       denominator += factor;
                     }
                     doNotOptimize(numerator / denominator);
                   });
                 });
}

/**
 * Speed of light for addLinearTG (perf_interp_sol.hpp): the same 2^d source-function calls,
 * corners in Gray-code order, weights from a table built by doubling; integrandOnly: the
 * source-function calls alone (bracketing and mapping of the neighbors included).
 */
void addSolLinearTG(Suite& suite, const size_t nDim, const LvlType maxLvl,
                    const bool integrandOnly) {
  const std::string kind = integrandOnly ? "interplin_tg_integrand/" : "interplin_tg/";
  suite.addMicro("micro/sol/" + kind + cfg("equi", 1, nDim, maxLvl), [=](MicroRun& run) {
    const HyperCubeArea domain = perf::shiftedDomain(nDim);
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, "equi", 1, domain);
    const SparseGrid sg(genInstr);
    const SourceFunc sourceFunc(perf::cosProd);
    const DataVector point = perf::randomPoints(domain, 1, 98)[0];
    const DataVector normPoint = cg::tools::normalizeDataVector(point, domain);
    const TensorGrid& tg = largestTensorGrid(sg).tensorGrid;
    sol::LinScratch scratch(nDim);

    run.setItems(std::pow(2.0, static_cast<double>(nDim)));
    run.measure([&]() {
      doNotOptimize(
          sol::linearKernelTG(sourceFunc, tg, normPoint, domain, !integrandOnly, scratch));
    });
  });
}

/// The m scalar functions cosProdK(., k) and the VectorSourceFunc of all of them.
std::vector<SourceFunc> cosProdScalarFuncs(const size_t m) {
  std::vector<SourceFunc> funcs;
  for (size_t k = 0; k < m; k++) {
    funcs.emplace_back([k](const DataVector& x) { return cosProdK(x, k); });
  }
  return funcs;
}

cg::VectorSourceFunc cosProdVectorFunc(const size_t m) {
  return cg::VectorSourceFunc(
      [](const DataVector& x, std::span<double> out) {
        for (size_t k = 0; k < out.size(); k++) out[k] = cosProdK(x, k);
      },
      m);
}

/**
 * The linear interpolation operator interpolateLinear on a DeltasSparseGrid (every distinct corner
 * evaluated once) at the query point of addLinearSGOperator (items: corners of all combination
 * tensor grids, as there); m > 0: a VectorSourceFunc with m outputs (micro/interplin/dsg_vec, with
 * the SparseGrid operator as micro/interplin/sg_vec).
 */
void addLinearDSGOperator(Suite& suite, const size_t nDim, const LvlType maxLvl, const size_t m) {
  const std::string suffix = m == 0 ? "" : "/m" + std::to_string(m);
  for (const bool deltas : {false, true}) {
    if (!deltas && m == 0) continue;  // micro/interplin/sg (addLinearSGOperator)
    const std::string kind = std::string(deltas ? "dsg" : "sg") + (m == 0 ? "/" : "_vec/");
    suite.addMicro(
        "micro/interplin/" + kind + cfg("equi", 1, nDim, maxLvl) + suffix, [=](MicroRun& run) {
          const HyperCubeArea domain = perf::shiftedDomain(nDim);
          CompleteSGGenInstr genInstr(maxLvl, nDim);
          perf::configure(genInstr, "equi", 1, domain);
          const SparseGrid sg(genInstr);
          const DeltasSparseGrid dsg(genInstr);
          const SourceFunc sourceFunc(perf::cosProd);
          const cg::VectorSourceFunc vecFunc = cosProdVectorFunc(std::max<size_t>(m, 1));
          const DataVector point = perf::randomPoints(domain, 1, 98)[0];
          std::vector<double> out(m);

          run.setItems(static_cast<double>(sg.nTG()) * std::pow(2.0, static_cast<double>(nDim)));
          run.measure([&]() {
            if (m == 0) {
              doNotOptimize(cg::interpolateLinear(sourceFunc, point, dsg));
            } else if (deltas) {
              cg::interpolateLinear(vecFunc, point, dsg, out);
              doNotOptimize(out[0]);
            } else {
              cg::interpolateLinear(vecFunc, point, sg, out);
              doNotOptimize(out[0]);
            }
          });
        });
  }
}

/**
 * The global interpolation operator interpolate on a DeltasSparseGrid (every grid point evaluated
 * once, weighted with its global weight) at the query point of addGlobalSGOperator (items: grid
 * points of all tensor grids of the SparseGrid, as there); m > 0: a VectorSourceFunc with m
 * outputs (micro/interp/dsg_vec, with the SparseGrid operator as micro/interp/sg_vec).
 */
void addGlobalDSGOperator(Suite& suite, const std::string& nodes, const size_t nDim,
                          const LvlType maxLvl, const size_t m) {
  const std::string suffix = m == 0 ? "" : "/m" + std::to_string(m);
  for (const bool deltas : {false, true}) {
    if (!deltas && m == 0) continue;  // micro/interp/sg (addGlobalSGOperator)
    const std::string kind = std::string(deltas ? "dsg" : "sg") + (m == 0 ? "/" : "_vec/");
    suite.addMicro("micro/interp/" + kind + cfg(nodes, 0, nDim, maxLvl) + suffix,
                   [=](MicroRun& run) {
                     const HyperCubeArea domain = perf::shiftedDomain(nDim);
                     CompleteSGGenInstr genInstr(maxLvl, nDim);
                     perf::configure(genInstr, nodes, 0, domain);
                     const SparseGrid sg(genInstr);
                     const DeltasSparseGrid dsg(genInstr);
                     const SourceFunc sourceFunc(perf::cosProd);
                     const cg::VectorSourceFunc vecFunc = cosProdVectorFunc(std::max<size_t>(m, 1));
                     const DataVector point = perf::randomPoints(domain, 1, 97)[0];
                     std::vector<double> out(m);

                     run.setItems(sumOfGridPoints(sg));
                     run.measure([&]() {
                       if (m == 0) {
                         doNotOptimize(cg::interpolate(sourceFunc, point, dsg));
                       } else if (deltas) {
                         cg::interpolate(vecFunc, point, dsg, out);
                         doNotOptimize(out[0]);
                       } else {
                         cg::interpolate(vecFunc, point, sg, out);
                         doNotOptimize(out[0]);
                       }
                     });
                   });
  }
}

/// Sparse grid, query point and largest tensor grid of the vector-valued interpolation cases.
struct VectorInterpSetup {
  VectorInterpSetup(const std::string& nodes, const LvlType boundary, const size_t nDim,
                    const LvlType maxLvl, const unsigned seed)
      : domain(perf::shiftedDomain(nDim)), genInstr(maxLvl, nDim) {
    perf::configure(genInstr, nodes, boundary, domain);
    sg.emplace(genInstr);
    const DataVector point = perf::randomPoints(domain, 1, seed)[0];
    normPoint = cg::tools::normalizeDataVector(point, domain);
    tgData = &largestTensorGrid(*sg);
  }

  HyperCubeArea domain;
  CompleteSGGenInstr genInstr;
  std::optional<SparseGrid> sg;
  DataVector normPoint;
  const TensorGridCTData* tgData = nullptr;
};

/**
 * WP-29c: global interpolation of a vector-valued source function with the production kernels
 * on the largest tensor grid (the micro/interp/global_tg configurations), ns per grid point and
 * output: micro/interp/global_tg_vec/<cfg>/m<m> (one VectorSourceFunc with the m outputs
 * cosProdK) vs. micro/interp/global_tg_scalar_x_m/<cfg>/m<m> (m scalar runs). Counter
 * max_ulp: vector vs. scalar results.
 */
void addVectorInterpTG(Suite& suite, const std::string& nodes, const size_t nDim,
                       const LvlType maxLvl, const size_t m) {
  const std::string name = cfg(nodes, 0, nDim, maxLvl) + "/m" + std::to_string(m);

  suite.addMicro("micro/interp/global_tg_scalar_x_m/" + name, [=](MicroRun& run) {
    const VectorInterpSetup s(nodes, 0, nDim, maxLvl, 99);
    const std::vector<SourceFunc> funcs = cosProdScalarFuncs(m);
    const cg::SGGenInstr& genInstr = s.sg->getGenInstr();

    run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP() * m));
    run.measure([&]() {
      for (const SourceFunc& f : funcs) {
        doNotOptimize(cg::tools::global_interpolation::processTensorGrid(f, s.normPoint, *s.tgData,
                                                                         genInstr));
      }
    });
  });

  suite.addMicro("micro/interp/global_tg_vec/" + name, [=](MicroRun& run) {
    const VectorInterpSetup s(nodes, 0, nDim, maxLvl, 99);
    const cg::VectorSourceFunc f = cosProdVectorFunc(m);
    const cg::SGGenInstr& genInstr = s.sg->getGenInstr();
    std::vector<double> result(m), scalars;

    cg::tools::global_interpolation::processTensorGrid(f, s.normPoint, *s.tgData, genInstr, result);
    for (const SourceFunc& fk : cosProdScalarFuncs(m)) {
      scalars.push_back(
          cg::tools::global_interpolation::processTensorGrid(fk, s.normPoint, *s.tgData, genInstr));
    }
    compareResults(run, name, result, scalars);

    run.setItems(static_cast<double>(s.tgData->tensorGrid.nGP() * m));
    run.measure([&]() {
      cg::tools::global_interpolation::processTensorGrid(f, s.normPoint, *s.tgData, genInstr,
                                                         result);
      doNotOptimize(result.data());
      perf::clobberMemory();
    });
  });
}

/**
 * WP-29c: linear interpolation of a vector-valued source function with the production kernels
 * on the largest tensor grid (the micro/interplin/tg configurations), ns per corner and output:
 * micro/interplin/tg_vec/<cfg>/m<m> vs. micro/interplin/tg_scalar_x_m/<cfg>/m<m> (as
 * addVectorInterpTG).
 */
void addVectorLinearTG(Suite& suite, const size_t nDim, const LvlType maxLvl, const size_t m) {
  const std::string name = cfg("equi", 1, nDim, maxLvl) + "/m" + std::to_string(m);
  const double nCorners = std::pow(2.0, static_cast<double>(nDim));

  suite.addMicro("micro/interplin/tg_scalar_x_m/" + name, [=](MicroRun& run) {
    const VectorInterpSetup s("equi", 1, nDim, maxLvl, 98);
    const std::vector<SourceFunc> funcs = cosProdScalarFuncs(m);
    const cg::SGGenInstr& genInstr = s.sg->getGenInstr();
    cg::misc::LinInterpScratch scratch(nDim);

    run.setItems(nCorners * static_cast<double>(m));
    run.measure([&]() {
      for (const SourceFunc& f : funcs) {
        doNotOptimize(cg::tools::lin_interpolation::processTensorGrid(f, s.normPoint, *s.tgData,
                                                                      genInstr, scratch));
      }
    });
  });

  suite.addMicro("micro/interplin/tg_vec/" + name, [=](MicroRun& run) {
    const VectorInterpSetup s("equi", 1, nDim, maxLvl, 98);
    const cg::VectorSourceFunc f = cosProdVectorFunc(m);
    const cg::SGGenInstr& genInstr = s.sg->getGenInstr();
    cg::misc::LinInterpScratch scratch(nDim, m), scalarScratch(nDim);
    std::vector<double> result(m), scalars;

    cg::tools::lin_interpolation::processTensorGrid(f, s.normPoint, *s.tgData, genInstr, scratch,
                                                    result);
    for (const SourceFunc& fk : cosProdScalarFuncs(m)) {
      scalars.push_back(cg::tools::lin_interpolation::processTensorGrid(fk, s.normPoint, *s.tgData,
                                                                        genInstr, scalarScratch));
    }
    compareResults(run, name, result, scalars);

    run.setItems(nCorners * static_cast<double>(m));
    run.measure([&]() {
      cg::tools::lin_interpolation::processTensorGrid(f, s.normPoint, *s.tgData, genInstr, scratch,
                                                      result);
      doNotOptimize(result.data());
      perf::clobberMemory();
    });
  });
}

/*******************************************************************************
Generation
*******************************************************************************/

/// Level multi-indices and coefficients of a complete sparse grid.
void addCompleteMIs(Suite& suite, const size_t nDim, const LvlType maxLvl) {
  suite.addMicro(
      "micro/gen/complete_mis_with_coeffs/d" + std::to_string(nDim) + "/L" + std::to_string(maxLvl),
      [=](MicroRun& run) {
        const CompleteSGGenInstr genInstr(maxLvl, nDim);
        size_t nMI = genInstr.genReducedMIVecWithCoeffs().first.nMI();

        run.setItems(static_cast<double>(nMI));
        run.measure([&]() {
          const auto misWithCoeffs = genInstr.genReducedMIVecWithCoeffs();
          doNotOptimize(misWithCoeffs.second.data());
        });
      });
}

/// Construction of a complete SparseGrid (multi-indices, coefficients, tensor grids).
void addSparseGridConstruction(Suite& suite, const std::string& nodes, const LvlType boundary,
                               const size_t nDim, const LvlType maxLvl) {
  suite.addMicro("micro/gen/sparse_grid/" + cfg(nodes, boundary, nDim, maxLvl), [=](MicroRun& run) {
    CompleteSGGenInstr genInstr(maxLvl, nDim);
    perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
    run.setItems(static_cast<double>(SparseGrid(genInstr).nTG()));
    run.measure([&]() {
      const SparseGrid sg(genInstr);
      doNotOptimize(sg.nTG());
    });
  });
}

/// Downwards closure of an anisotropic multi-index set (fresh copy per call: no cached data).
void addClosure(Suite& suite, const size_t nDim, const LvlType maxSum, const size_t nMI) {
  suite.addMicro("micro/mivec/downwards_closure/d" + std::to_string(nDim) + "/S" +
                     std::to_string(maxSum) + "/n" + std::to_string(nMI),
                 [=](MicroRun& run) {
                   const LvlMIVec pristine = perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim);
                   run.setItems(static_cast<double>(LvlMIVec(pristine).downwardsClosure().nMI()));
                   run.measure([&]() {
                     const LvlMIVec copy(pristine);
                     doNotOptimize(copy.downwardsClosure().nMI());
                   });
                 });
}

/// Pareto maxima of a not downwards-closed anisotropic set (fresh copy per call: no cached data).
void addParetoMaxima(Suite& suite, const size_t nDim, const LvlType maxSum, const size_t nMI) {
  suite.addMicro("micro/mivec/pareto_maxima/d" + std::to_string(nDim) + "/S" +
                     std::to_string(maxSum) + "/n" + std::to_string(nMI),
                 [=](MicroRun& run) {
                   const LvlMIVec pristine = perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim);
                   run.setItems(static_cast<double>(nMI));
                   run.measure([&]() {
                     const LvlMIVec copy(pristine);
                     doNotOptimize(copy.paretoMaxima()->size());
                   });
                 });
}

/// Combination coefficients of a downwards-closed set (fresh copy per call: no cached lookup).
void addCTCoeffs(Suite& suite, const size_t nDim, const LvlType maxSum, const size_t nMI) {
  suite.addMicro(
      "micro/mivec/ct_coeffs/d" + std::to_string(nDim) + "/S" + std::to_string(maxSum) + "/n" +
          std::to_string(nMI),
      [=](MicroRun& run) {
        const LvlMIVec closed =
            LvlMIVec(perf::anisotropicMIs(nDim, nMI, maxSum, 2000 + nDim)).downwardsClosure();
        const LvlMIVec pristine(closed);
        run.setItems(static_cast<double>(closed.nMI()));
        run.measure([&]() {
          const LvlMIVec copy(pristine);
          doNotOptimize(cg::tools::computeCTCoeffs(copy).data());
        });
      });
}

}  // namespace

int main(int argc, char** argv) {
  Suite suite("micro", 5, 0);  // the batch calibration doubles as warm-up

  // Complete sparse grid quadrature
  addProcessTG(suite, "equi", 0, 2, 16);
  addProcessTG(suite, "equi", 0, 5, 10);
  addProcessTG(suite, "equi", 0, 10, 4);
  addProcessTG(suite, "cc", 0, 2, 16);
  addProcessTG(suite, "cc", 0, 10, 4);
  addProcessAllTGs(suite, "equi", 1, 20, 4);
  addProcessAllTGs(suite, "cc", 1, 10, 5);  // WP-01: many short rows (levels 0 have 1 node)
  addSolQuadTG(suite, "equi", 0, 5, 10);
  addSolQuadTG(suite, "equi", 0, 10, 4);
  addIterateTG(suite, 10, 4);
  addEvalNormalized(suite, 10);
  addCCWeights(suite, 33);
  addCCWeights(suite, 1025);
  addCCWeights(suite, 32769);

  // Deltas sparse grid quadrature
  addProcessDeltaTG(suite, "equi", 0, 5, 12);
  addProcessDeltaTG(suite, "equi", 0, 10, 7);
  addProcessDeltaTG(suite, "equi", 1, 20, 6);
  addDeltaPrep(suite, "equi", 1, 20, 6);
  addSubTries(suite, "equi", 1, 20, 6, 256);  // WP-20: FlatNestedVec reads of the trie work
  addSubTries(suite, "equi", 0, 10, 7, 16);
  addProcessDeltaTGs(suite, "equi", 1, 20, 6, 256);  // WP-02: sub-tries + per-row updates
  addProcessDeltaTGs(suite, "equi", 1, 15, 7, 128);
  addProcessDeltaTGs(suite, "equi", 0, 10, 6, 64);
  // WP-04: the b0 row loop (rows of 2-4 grid points) at the e2e configurations, and b1
  addProcessDeltaTGs(suite, "equi", 0, 10, 7, 256);
  addProcessDeltaTGs(suite, "equi", 0, 10, 7, 256, true);
  addProcessDeltaTGs(suite, "equi", 0, 7, 10, 64, true);
  addProcessDeltaTGs(suite, "cc", 0, 7, 10, 64, true);
  addProcessDeltaTGs(suite, "equi", 1, 20, 6, 256, true);

  // Interpolation
  addInterpTG(suite, "cc", 5, 6);
  addInterpTG(suite, "cheb1", 5, 4);
  addInterpTG(suite, "cc", 10, 2);  // WP-29c: high / low d, equi (generic formula)
  addInterpTG(suite, "cc", 2, 12);
  addInterpTG(suite, "equi", 3, 5);
  // WP-45: the whole operator at one query point (SparseGrid; control of micro/interp/dsg)
  addGlobalSGOperator(suite, "cc", 8, 2);
  addGlobalSGOperator(suite, "cc", 5, 5);
  addGlobalSGOperator(suite, "cc", 3, 8);
  addGlobalSGOperator(suite, "cc", 2, 12);
  addGlobalSGOperator(suite, "equi", 3, 5);
  addGlobalSGOperator(suite, "cheb1", 5, 4);  // WP-06: generic formula, many tensor grids
  // WP-45: the deltas sparse grid operator (same configurations), vector-valued with m = 4
  addGlobalDSGOperator(suite, "cc", 8, 2, 0);
  addGlobalDSGOperator(suite, "cc", 5, 5, 0);
  addGlobalDSGOperator(suite, "cc", 3, 8, 0);
  addGlobalDSGOperator(suite, "cc", 2, 12, 0);
  addGlobalDSGOperator(suite, "equi", 3, 5, 0);
  addGlobalDSGOperator(suite, "cc", 5, 5, 4);
  addGlobalDSGOperator(suite, "cc", 2, 12, 4);
  addBarycentric1D(suite, false, 9);
  addBarycentric1D(suite, false, 65);
  addBarycentric1D(suite, true, 9);
  addBarycentric1D(suite, true, 65);
  addLinearTG(suite, 8, 5);
  addLinearTG(suite, 10, 5);  // WP-29c: high / low d
  addLinearTG(suite, 2, 16);
  addLinearTG(suite, 5, 8);
  addLinearTG(suite, 16, 2);  // 2^16 corners per tensor grid
  // WP-47: the whole operator at one query point (SparseGrid; control of micro/interplin/dsg)
  addLinearSGOperator(suite, 10, 5);
  addLinearSGOperator(suite, 8, 5);
  addLinearSGOperator(suite, 5, 8);
  addLinearSGOperator(suite, 2, 16);
  // WP-47: the deltas sparse grid operator (same configurations), vector-valued with m = 4
  addLinearDSGOperator(suite, 10, 5, 0);
  addLinearDSGOperator(suite, 8, 5, 0);
  addLinearDSGOperator(suite, 5, 8, 0);
  addLinearDSGOperator(suite, 2, 16, 0);
  addLinearDSGOperator(suite, 8, 5, 4);
  addLinearDSGOperator(suite, 2, 16, 4);
  // Speed of light of the per-TG interpolation kernels and of the generic barycentric formula
  // with cached weights (perf_interp_sol.hpp)
  for (const bool integrandOnly : {false, true}) {
    addSolInterpTG(suite, "cc", 10, 2, integrandOnly);
    addSolInterpTG(suite, "cc", 5, 6, integrandOnly);
    addSolInterpTG(suite, "cheb1", 5, 4, integrandOnly);
    addSolInterpTG(suite, "cc", 2, 12, integrandOnly);
    addSolLinearTG(suite, 10, 5, integrandOnly);
    addSolLinearTG(suite, 8, 5, integrandOnly);
    addSolLinearTG(suite, 2, 16, integrandOnly);
    addSolLinearTG(suite, 5, 8, integrandOnly);
    addSolLinearTG(suite, 16, 2, integrandOnly);
  }
  addSolBarycentricCached(suite, 9);
  addSolBarycentricCached(suite, 65);

  // Generation
  addCompleteMIs(suite, 20, 6);
  addSparseGridConstruction(suite, "cc", 1, 20, 5);
  addClosure(suite, 10, 10, 300);
  addClosure(suite, 20, 6, 100);
  addClosure(suite, 5, 16, 1000);  // WP-03: the MI sets of the mivec e2e cases
  addClosure(suite, 10, 12, 1000);
  addClosure(suite, 15, 10, 300);
  addClosure(suite, 20, 7, 300);
  addParetoMaxima(suite, 10, 12, 1000);
  addParetoMaxima(suite, 20, 7, 300);
  addCTCoeffs(suite, 10, 12, 1000);

  // WP-29a: vector-valued SG quadrature, production kernels (the WP-24 configurations)
  for (const size_t m : {1, 2, 4, 8, 32}) {
    addVectorQuad(suite, "equi", 0, 2, 16, false, m);
    addVectorQuad(suite, "equi", 0, 10, 4, false, m);
    addVectorQuad(suite, "equi", 1, 20, 4, true, m);
  }

  // WP-29b: vector-valued DSG quadrature, production kernels (the WP-24 configurations, b1 d20
  // also over every 256th delta tensor grid, as micro/dsg/process_delta_tgs)
  for (const size_t m : {1, 2, 4, 8, 32}) {
    addVectorDeltaQuad(suite, "equi", 0, 10, 7, 0, m);
    addVectorDeltaQuad(suite, "equi", 1, 20, 6, 0, m);
    addVectorDeltaQuad(suite, "equi", 1, 20, 6, 256, m);
  }

  // WP-29c: vector-valued global / linear interpolation, production kernels (the
  // micro/interp/global_tg and micro/interplin/tg configurations)
  for (const size_t m : {1, 2, 4, 8, 32}) {
    addVectorInterpTG(suite, "cc", 2, 12, m);
    addVectorInterpTG(suite, "cc", 5, 6, m);
    addVectorInterpTG(suite, "cheb1", 5, 4, m);
    addVectorInterpTG(suite, "cc", 10, 2, m);
    addVectorLinearTG(suite, 2, 16, m);
    addVectorLinearTG(suite, 8, 5, m);
    addVectorLinearTG(suite, 10, 5, m);
  }

  // WP-25: source-function cache, references (micro/cache/{hash,lookup,insert}/..., the
  // flat-file disk tier; the WP-24 / WP-25 prototypes were removed in WP-40)
  perf::cache_proto::addCacheProtoCases(suite);

#ifdef SGPP_COMBIGRID_COMPILE_BOOST_POINT_VALUE_TABLE
  // WP-26: the production source-function cache (micro/cache/.../production...)
  perf::cache_production::addCacheProductionCases(suite);
#endif

  return suite.main(argc, argv);
}
