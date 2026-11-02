// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file adaptive_cache_mode.hpp
 * @brief What a source-function cache with @c CachePolicy::Adaptive does at present.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief The policy a @c SourceFuncCache with @c CachePolicy::Adaptive applies at present
 * (@c CacheStatistics::adaptiveMode). Each mode uses only its own tiers: values stored by
 * another mode stay, but are only found again once a mode that uses their tier is back.
 * Python: @c pysgpp.AdaptiveCacheMode_None, ...
 */
enum class AdaptiveCacheMode {
  /// No caching: the function is evaluated directly (no lookup, nothing stored).
  None,
  /// As @c CachePolicy::MemoryOnly (the start mode).
  MemoryOnly,
  /// As @c CachePolicy::TemporaryStorage: values beyond the memory limit go to a temporary file,
  /// created at the first such value.
  TemporaryStorage,
};

}  // namespace combigrid
}  // namespace sgpp
