// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <sgpp/combigrid/tools/defines/dependencies_defines.hpp>

#ifdef SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE

#include <sgpp/combigrid/miscellaneous/concurrency/spin_lock.hpp>

namespace sgpp {
namespace combigrid {

namespace misc {
namespace lmdb_point_value_store {

/// A reusable read transaction (reset between lookups).
struct alignas(64) ReadSlot {
  SpinLock lock;
  MDB_txn* txn = nullptr;
};

}  // namespace lmdb_point_value_store
}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp

#endif  // SGPP_COMBIGRID_COMPILE_LMDB_POINT_VALUE_STORE
