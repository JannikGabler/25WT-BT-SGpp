// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file lmdb_point_value_store_read_slot.hpp
 * @brief Reusable read transaction of a @c misc::LMDBPointValueStore.
 *
 * Internal header (includes @c lmdb.h; only with
 * @c SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE).
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <lmdb.h>

#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/**
 * @brief A read transaction (@c MDB_RDONLY, @c MDB_NOTLS) that lookups renew and reset, so that
 * a lookup does not begin a transaction; the lock serializes the lookups of one slot. One cache
 * line per slot (no false sharing between the slots of different shards).
 */
struct alignas(64) ReadSlot {
  SpinLock lock;           ///< Serializes the lookups of the slot.
  MDB_txn* txn = nullptr;  ///< The transaction (reset while unused).
};

}  // namespace lmdb_point_value_store
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
