// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org
#pragma once

#include <cstddef>
#include <cstdint>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace source_func_caching {

/**
 * @brief Layout of a record of the source-function cache in 8-byte words:
 * @c [hash | point (nDim doubles) | values (nOutputs doubles)].
 */
struct RecordLayout {
  size_t nDim;      ///< Number of coordinates d of a point.
  size_t nOutputs;  ///< Number of values m per point.

  /// @brief Number of 8-byte words of a record.
  size_t stride() const { return 1 + nDim + nOutputs; }

  /// @brief Number of bytes of a record.
  size_t bytes() const { return sizeof(uint64_t) * stride(); }
};

}  // namespace source_func_caching
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
