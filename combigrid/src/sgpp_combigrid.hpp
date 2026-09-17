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
 * scratch buffers, caching, concurrency, per-operator internals) are
 * intentionally not included.
 */

#pragma once

// Type aliases and multi-indices
#include <sgpp/combigrid/type_defs.hpp>
#include <sgpp/combigrid/multiindices/multiindex.hpp>
#include <sgpp/combigrid/multiindices/multiindex_vector.hpp>

// Source functions
#include <sgpp/combigrid/functions/source_functions/source_function.hpp>

// Level-to-grid-point-count (growth) functions
#include <sgpp/combigrid/functions/level_to_grid_point_count_functions/level_to_grid_point_count_functions.hpp>

// Node generation functions and their singleton getters
#include <sgpp/combigrid/functions/node_generation_functions/node_generation_function.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/clenshaw_curtis_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/equidistant_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/first_type_chebyshev_node_generation_function_getter.hpp>
#include <sgpp/combigrid/functions/node_generation_functions/getter/second_type_chebyshev_node_generation_function_getter.hpp>

// Sparse grid generation instructions
#include <sgpp/combigrid/sparse_grid_generation_instructions/sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/complete_sg_gen_instruction.hpp>
#include <sgpp/combigrid/sparse_grid_generation_instructions/multiindex_vector_sg_gen_instruction.hpp>

// Grids
#include <sgpp/combigrid/grids/tensor_grid.hpp>
#include <sgpp/combigrid/miscellaneous/tensor_grid/tensor_grid_combination_technique_data.hpp>
#include <sgpp/combigrid/grids/sparse_grid.hpp>

// Operators
#include <sgpp/combigrid/operators/global_interpolation/global_interpolation.hpp>
#include <sgpp/combigrid/operators/linear_interpolation/linear_interpolation.hpp>
#include <sgpp/combigrid/operators/quadrature/quadrature.hpp>

// Tools
#include <sgpp/combigrid/tools/combitech_coefficients/combitech_coefficients.hpp>
