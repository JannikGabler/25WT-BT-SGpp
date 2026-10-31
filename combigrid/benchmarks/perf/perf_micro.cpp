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
#include <cmath>
#include <cstddef>
#include <optional>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/miscellaneous/ct_coefficient_trie/ct_coefficient_trie.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/delta_quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_contexts/quadrature_context.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/delta_quadrature_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/linear_interpolation_scratch.hpp>
#include <sgpp/combigrid/miscellaneous/data_scratches/quadrature_scratch.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/global_interpolation/methods/getters/optimized_barycentric_formula_getter.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/getters/clenshaw_curtis_quadrature_rule_getter.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_rules/quadrature_rule.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
#include <sgpp/combigrid/tools/data_vector/normalization.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/deltas_sparse_grid_quadrature_tools.hpp>
#include <sgpp/combigrid/tools/operators/quadrature/quadrature_tools.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <span>
#include <string>
#include <vector>

#include "perf_cache_proto.hpp"
#include "perf_harness.hpp"
#include "perf_problems.hpp"
#include "perf_vector_proto.hpp"

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

namespace {

std::string cfg(const std::string& nodes, const LvlType boundary, const size_t nDim,
                const LvlType maxLvl) {
  return nodes + "/b" + std::to_string(boundary) + "/d" + std::to_string(nDim) + "/L" +
         std::to_string(maxLvl);
}

template <typename Grid>
const TensorGridCTData& largestTensorGrid(const Grid& grid) {
  const TensorGridCTData* largest = &*grid.begin();
  for (const TensorGridCTData& tgData : grid) {
    if (tgData.tensorGrid.nGP() > largest->tensorGrid.nGP()) largest = &tgData;
  }
  return *largest;
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
    tgs = perf::vec_proto::selectTGs(*sg, allTGs);
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
 * outputs vec_proto::expSumK) vs. micro/quad/<kernel>_scalar_x_m/<cfg>/m<m> (m scalar runs, one
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
      funcs.emplace_back([k](const DataVector& x) { return perf::vec_proto::expSumK(x, k); });
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
          for (size_t k = 0; k < out.size(); k++) out[k] = perf::vec_proto::expSumK(x, k);
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
        const SourceFunc fk([k](const DataVector& x) { return perf::vec_proto::expSumK(x, k); });
        scalarPerTG.push_back(cg::tools::quadrature::processTensorGrid(
            context, fk, tgData->tensorGrid, scalarScratch));
      }
    }
    perf::vec_proto::compareResults(run, name, vecPerTG, scalarPerTG);

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
        const cg::misc::DeltaQuadratureContext context{.deltasSparseGrid = dsg,
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
        const cg::misc::DeltaQuadratureContext context{.deltasSparseGrid = dsg,
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
void addProcessDeltaTGs(Suite& suite, const std::string& nodes, const LvlType boundary,
                        const size_t nDim, const LvlType maxLvl, const size_t stride) {
  suite.addMicro(
      "micro/dsg/process_delta_tgs/" + cfg(nodes, boundary, nDim, maxLvl) + "/s" +
          std::to_string(stride),
      [=](MicroRun& run) {
        CompleteSGGenInstr genInstr(maxLvl, nDim);
        perf::configure(genInstr, nodes, boundary, perf::unitDomain(nDim));
        const DeltasSparseGrid dsg(genInstr);
        const SourceFunc sourceFunc(perf::expSum);
        const cg::misc::FlatNestedVec<double, 3> weights1D =
            cg::tools::dsg_quadrature::comp1DWeights(dsg);
        const cg::misc::CTCoeffTrie coeffTrie(dsg.getTensorGrids(), nDim);
        const cg::misc::DeltaQuadratureContext context{.deltasSparseGrid = dsg,
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

  cg::misc::DeltaQuadratureContext context() const {
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
 * outputs vec_proto::expSumK) vs. micro/dsg/<kernel>_scalar_x_m/<cfg>/m<m> (m scalar runs, one
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
    const cg::misc::DeltaQuadratureContext context = s.context();
    cg::misc::DeltaQuadratureScratch scratch(nDim, s.dsg->getMaxTGSumOverGPCntsPerDim());
    std::vector<SourceFunc> funcs;
    for (size_t k = 0; k < m; k++) {
      funcs.emplace_back([k](const DataVector& x) { return perf::vec_proto::expSumK(x, k); });
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
    const cg::misc::DeltaQuadratureContext context = s.context();
    cg::misc::DeltaQuadratureScratch scratch(nDim, s.dsg->getMaxTGSumOverGPCntsPerDim(), m);
    const cg::VectorSourceFunc f(
        [](const DataVector& x, std::span<double> out) {
          for (size_t k = 0; k < out.size(); k++) out[k] = perf::vec_proto::expSumK(x, k);
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
        const SourceFunc fk([k](const DataVector& x) { return perf::vec_proto::expSumK(x, k); });
        scalarPerTG.push_back(
            cg::tools::dsg_quadrature::processDeltaTensorGrid(context, fk, *tgData, scalarScratch));
      }
    }
    perf::vec_proto::compareResults(run, name, vecPerTG, scalarPerTG);

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
      doNotOptimize(
          cg::global_interpolation::interpolate(sourceFunc, normPoint, tgData, sg.getGenInstr()));
    });
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
      doNotOptimize(cg::linear_interpolation::interpolate(sourceFunc, normPoint, tgData,
                                                          sg.getGenInstr(), scratch));
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

  // Interpolation
  addInterpTG(suite, "cc", 5, 6);
  addInterpTG(suite, "cheb1", 5, 4);
  addBarycentric1D(suite, false, 9);
  addBarycentric1D(suite, false, 65);
  addBarycentric1D(suite, true, 9);
  addBarycentric1D(suite, true, 65);
  addLinearTG(suite, 8, 5);

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

  // WP-24: vector-valued source functions (prototypes, micro/vec/...)
  perf::vec_proto::addVectorProtoCases(suite);

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

  // WP-25: source-function cache (prototypes, micro/cache/...)
  perf::cache_proto::addCacheProtoCases(suite);

  return suite.main(argc, argv);
}
