#pragma once

#include <cstddef>

// Pareto Maximum calculations

namespace sgpp {
namespace combigrid {

namespace constants {

namespace mi_vec {

// Component wise max
constexpr size_t CWM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000;

// Pareto maxima
constexpr size_t PM_MIN_MIVEC_LENGTH_FOR_CONCURRENCY = 10000;
constexpr size_t PM_MIN_MIVEC_BATCH_LENGTH_PER_THREAD = 5000;

// Downwards closedness
constexpr size_t DWC_MIN_MI_FOR_CONCURRENCY = 10000;

}  // namespace mi_vec

namespace source_func {

constexpr bool USE_CACHE = false;

}

namespace sg_gen_instr {

// Full sg gen instruction
constexpr size_t FSG_MIN_MI_FOR_CONCURRENCY = 1000;
constexpr size_t FSG_MIN_MI_PER_THREAD = 500;

}  // namespace sg_gen_instr

namespace ct_coefficients {

constexpr size_t MIN_MIS_FOR_CONCURRENCY = 1000;

}

namespace sg_gen_node_lookup {

constexpr size_t FOR_ITERATIONS_TO_TASK_QUEUE_PUSH = 100;

}

}  // namespace constants

}  // namespace combigrid
}  // namespace sgpp
