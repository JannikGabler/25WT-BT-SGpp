// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file arithmetic.hpp
 * @brief Defines the concepts @c Arithmetic (integer and floating-point types without @c bool)
 * and @c ArithmeticRange (contiguous sized ranges of them).
 */
#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Satisfied by the integer and floating-point types except @c bool (cv-qualified or not):
 * the types whose values are numbers, e.g. coefficients and values of a sum.
 */
template <typename T>
concept Arithmetic =
    (std::integral<T> || std::floating_point<T>) && !std::same_as<std::remove_cv_t<T>, bool>;

/**
 * @brief Satisfied by contiguous sized ranges of @ref Arithmetic values, e.g. @c std::span,
 * @c std::vector, @c std::array and @c base::DataVector.
 */
template <typename R>
concept ArithmeticRange = std::ranges::contiguous_range<R> && std::ranges::sized_range<R> &&
                          Arithmetic<std::ranges::range_value_t<R>>;

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
