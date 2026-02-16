#include <cstddef>

// Pareto Maximum calculations

namespace sgpp {
namespace combigrid {

namespace pareto_maxima {

constexpr size_t MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000;

constexpr size_t MIN_MIVEC_BATCH_LENGTH_PER_THREAD = 5000;

}  // namespace pareto_maxima

namespace ct_coefficients {

constexpr size_t MIN_MIS_FOR_CONCURRENCY = 10000;

}

}  // namespace combigrid
}  // namespace sgpp
