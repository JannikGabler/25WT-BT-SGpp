// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file source_function_hook.hpp
 * @brief Interface of the end-of-operator hook of @c SourceFunc and @c VectorSourceFunc.
 */
#pragma once

namespace sgpp {
namespace combigrid {

/**
 * @brief Object attached to a source function that every operator notifies once at the end of a
 * call (e.g. a @c SourceFuncCache: statistics; deferred work such as disk writes).
 *
 * A plain source function has no hook. The operators read the hook once per call, never per
 * evaluation, so a plain source function costs nothing extra.
 */
class SourceFuncHook {
 public:
  virtual ~SourceFuncHook() = default;

  /**
   * @brief Called once at the end of every operator call with a source function this hook is
   * attached to (after the operator's parallel region, by the calling thread).
   */
  virtual void finishOperatorCall() = 0;
};

}  // namespace combigrid
}  // namespace sgpp
