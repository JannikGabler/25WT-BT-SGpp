#pragma once

#include <cstring>
#include <mutex>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/miscellaneous/hashing/data_vector_hash.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <unordered_map>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace misc {

class SourceFunctionCache {
 public:
  explicit SourceFunctionCache(size_t shard_count = 256, size_t per_shard_reserve = 0);

  // For now not copyable, but movable
  SourceFunctionCache(const SourceFunctionCache&) = delete;
  SourceFunctionCache& operator=(const SourceFunctionCache&) = delete;

  bool find(const base::DataVector& point, double& f_out) const;

  // Insert / overwrite
  void insert(const base::DataVector& point, double value);

  // Insert / overwrite
  void insert(base::DataVector&& point, double value);

  bool erase(const base::DataVector& point);

  // Sums over all shards (-> O(#shards))
  size_t size() const;

  size_t shardCnt() const noexcept;

  double findOrCompute(const base::DataVector& point, const SourceFunc sourceFunc);

  std::vector<std::unordered_map<base::DataVector, double, DataVectorHash>> dump() const;

 private:
  struct Shard {
    mutable std::mutex mutex;
    std::unordered_map<base::DataVector, double, DataVectorHash> map;
  };

  std::vector<Shard> shards_;
  DataVectorHash hasher_;

  size_t shard_index(const base::DataVector& v) const noexcept;
};

}  // namespace misc
}  // namespace combigrid
}  // namespace sgpp