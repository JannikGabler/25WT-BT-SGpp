// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file lmdb_point_value_store_file_header.hpp
 * @brief Header record of the file of a @c misc::LMDBPointValueStore.
 */
#pragma once

#include <cstdint>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/**
 * @brief Value of the key @c constants::lmdb_point_value_store::HEADER_KEY in the sub-database
 * @c meta (32 bytes, native byte order; copied with @c memcpy).
 */
struct FileHeader {
  char magic[8];       ///< @c constants::lmdb_point_value_store::MAGIC.
  uint32_t version;    ///< Format version (@c constants::lmdb_point_value_store::VERSION).
  uint32_t keyScheme;  ///< Key scheme (@c constants::lmdb_point_value_store::KEY_SCHEME).
  uint64_t nDim;       ///< Number of coordinates d of the points.
  uint64_t nOutputs;   ///< Number of values m per point.
};

}  // namespace lmdb_point_value_store
}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp