// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <atomic>
#include <cstddef>
#include <sgpp/combigrid/tools/operators/cross_call/cross_call_operator_data_tools.hpp>

namespace sgpp {
namespace combigrid {

namespace tools {
namespace cross_operator_call {

namespace {

std::atomic<size_t> nBuilds{0};  ///< See nOperatorDataBuilds.

}  // namespace

void countOperatorDataBuild() noexcept { nBuilds.fetch_add(1, std::memory_order_relaxed); }

size_t nOperatorDataBuilds() noexcept { return nBuilds.load(std::memory_order_relaxed); }

}  // namespace cross_operator_call
}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp
