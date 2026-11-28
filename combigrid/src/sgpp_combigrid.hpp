// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file sgpp_combigrid.hpp
 * @brief Umbrella header of the @c combigrid module.
 *
 * Including this header pulls in the public interface of the combination
 * technique module. The combination technique constructs a sparse grid
 * approximation as a linear combination of (anisotropic) tensor grids and
 * thereby avoids the curse of dimensionality.
 *
 * The set of headers listed here is exactly the API surface exposed to
 * Python by @c combigrid/build/pysgpp/combigrid.i (the SWIG wrapper is
 * compiled against this header). Internal helpers (bounding boxes, lookups,
 * scratch buffers, the table of the source-function cache, concurrency,
 * per-operator internals) are intentionally not included. The classes of the
 * source-function cache are always declared; they work if SG++ was built with
 * Boost >= 1.81 and xxHash >= 0.8, otherwise their constructors throw
 * @c base::dependency_exception.
 */

#pragma once

// Type aliases and multi-indices
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>
#include <sgpp/combigrid/type_defs.hpp>

// Source functions
#include <sgpp/combigrid/functions/source_functions/caching/adaptive_cache_mode.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_policy.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cache_statistics.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/cached_vector_source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache.hpp>
#include <sgpp/combigrid/functions/source_functions/caching/source_function_cache_config.hpp>
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>
#include <sgpp/combigrid/functions/source_functions/vector_source_function.hpp>

// Level-to-grid-point-count (growth) functions
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>

// Node families and their singleton getters
#include <sgpp/combigrid/node_families/getter/clenshaw_curtis_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/equidistant_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/first_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/getter/second_type_chebyshev_nodes_getter.hpp>
#include <sgpp/combigrid/node_families/node_family.hpp>

// Sparse grid generation instructions
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sparse_grid_generation_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/sparse_grid_generation_instruction.hpp>

// Grids
#include <sgpp/combigrid/grids/cross_call_operator_data_info.hpp>
#include <sgpp/combigrid/grids/deltas_sparse_grid.hpp>
#include <sgpp/combigrid/grids/node_row_key.hpp>
#include <sgpp/combigrid/grids/node_storage.hpp>
#include <sgpp/combigrid/grids/node_table.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>
#include <sgpp/combigrid/grids/sparse_grid_setup.hpp>
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/grids/tensor_grid_view.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data_view.hpp>

// Operators (SparseGrid and DeltasSparseGrid overloads)
#include <sgpp/combigrid/operators/global_interpolation/deltas_sparse_grid_global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation_options.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/deltas_sparse_grid_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation_options.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/deltas_sparse_grid_quadrature_options.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature_options.hpp>

// Textbook operators (SparseGrid only): plain per-tensor-grid reference implementations
#include <sgpp/combigrid/operators/textbook/textbook_global_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_linear_interpolation.hpp>
#include <sgpp/combigrid/operators/textbook/textbook_quadrature.hpp>

// Dimension-adaptive refinement (not bound in Python yet: the headers are not %include'd in
// combigrid.i)
#include <sgpp/combigrid/refinement/dimension_adaptive_refinement.hpp>
#include <sgpp/combigrid/refinement/refinement_indicator.hpp>
#include <sgpp/combigrid/refinement/refinement_options.hpp>
#include <sgpp/combigrid/refinement/refinement_schedule.hpp>
#include <sgpp/combigrid/refinement/refinement_statistics.hpp>
#include <sgpp/combigrid/refinement/refinement_stop_reason.hpp>
#include <sgpp/combigrid/refinement/targets/global_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/linear_interpolation_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/quadrature_refinement_target.hpp>
#include <sgpp/combigrid/refinement/targets/refinement_target.hpp>

// Tools
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
