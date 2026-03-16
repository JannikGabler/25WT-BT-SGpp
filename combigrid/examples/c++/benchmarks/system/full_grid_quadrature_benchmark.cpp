#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <ostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/full_sg_gen_instruction.hpp>
#include <vector>

using namespace sgpp::combigrid;
using DataVector = sgpp::base::DataVector;

static constexpr size_t nDim = 5;
static constexpr size_t maxLvl = 10;
static constexpr size_t warmupRuns = 1;
static constexpr size_t runs = 3;

SourceFunc getSinSourceFunc() {
  return SourceFunc([](const sgpp::base::DataVector& x) {
    double result = 1;

    for (size_t i = 0; i < nDim; i++) {
      result *= std::sin(x[i]);
    }

    return result;
  });
}

std::pair<double, std::array<double, 4>> runSingleBenchmark() {
  const auto start = std::chrono::high_resolution_clock::now();

  // Source func
  const SourceFunc sourceFunc = getSinSourceFunc();

  const auto afterSourceFunc = std::chrono::high_resolution_clock::now();

  // SG gen instr
  FullSGGenInstr genInstr(maxLvl, nDim);
  genInstr.setNodeGenFunc(getClenshawCurtisNodeGenFunc());

  const auto afterGenInstr = std::chrono::high_resolution_clock::now();

  // SG gen
  const SparseGrid sg(genInstr);

  const auto afterSG = std::chrono::high_resolution_clock::now();

  // Quadrature
  const double result = quadrature(sourceFunc, sg);

  const auto afterQuadrature = std::chrono::high_resolution_clock::now();

  const double sourceFuncTime = std::chrono::duration<double>(afterSourceFunc - start).count();
  const double genInstrTime =
      std::chrono::duration<double>(afterGenInstr - afterSourceFunc).count();
  const double sgGenTime = std::chrono::duration<double>(afterSG - afterGenInstr).count();
  const double quadratureTime = std::chrono::duration<double>(afterQuadrature - afterSG).count();

  return {result, {sourceFuncTime, genInstrTime, sgGenTime, quadratureTime}};
}

int main() {
  double avgTotalTime = 0;
  double avgSourceFuncTime = 0;
  double avgGenInstrTime = 0;
  double avgSGGenTime = 0;
  double avgQuadratureTime = 0;

  for (size_t i = 0; i < warmupRuns; i++) {
    std::cout << "- Warumup " << i << " -" << std::endl;
    const auto results = runSingleBenchmark();
    std::cout << "Result: " << results.first << std::endl;
  }
  std::cout << std::endl;

  for (size_t i = 0; i < runs; i++) {
    std::cout << "--- Run " << i << " ---" << std::endl;

    const auto results = runSingleBenchmark();
    const double totalTime =
        results.second[0] + results.second[1] + results.second[2] + results.second[3];

    std::cout << "Source func: " << results.second[0] << "s" << std::endl;
    std::cout << "Gen instr: " << results.second[1] << "s" << std::endl;
    std::cout << "SG gen: " << results.second[2] << "s" << std::endl;
    std::cout << "Quadrature: " << results.second[3] << "s" << std::endl;
    std::cout << "= " << totalTime << "s" << std::endl;
    std::cout << "Result: " << results.first << std::endl;

    avgTotalTime += totalTime;
    avgSourceFuncTime += results.second[0];
    avgGenInstrTime += results.second[1];
    avgSGGenTime += results.second[2];
    avgQuadratureTime += results.second[3];
  }
  std::cout << std::endl;

  avgTotalTime /= static_cast<double>(runs);
  avgSourceFuncTime /= static_cast<double>(runs);
  avgGenInstrTime /= static_cast<double>(runs);
  avgSGGenTime /= static_cast<double>(runs);
  avgQuadratureTime /= static_cast<double>(runs);

  std::cout << "### Averages ###" << std::endl;
  std::cout << "Source func: " << avgSourceFuncTime << "s" << std::endl;
  std::cout << "Gen instr: " << avgGenInstrTime << "s" << std::endl;
  std::cout << "SG gen: " << avgSGGenTime << "s" << std::endl;
  std::cout << "Quadrature: " << avgQuadratureTime << "s" << std::endl;
  std::cout << "= " << avgTotalTime << "s" << std::endl;
}
