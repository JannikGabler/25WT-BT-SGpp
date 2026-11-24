// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file cpp_concepts.hpp
 * @brief General-purpose C++ concepts of the module (type lists, subclasses, numeric types and
 * ranges), used to constrain templates.
 */
#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace sgpp {
namespace combigrid {

namespace misc {

/// @brief Satisfied if @p T is one of the types @p Ts (exactly, without removing cv / references).
template <typename T, typename... Ts>
concept OneOf = (std::same_as<T, Ts> || ...);

/**
 * @brief Satisfied if @c std::remove_cvref_t<T> is derived from @p Base and is not @p Base.
 *
 * Used to delete the converting constructors and assignments of a base class that would slice a
 * subclass object (e.g. @c SourceFunc from a @c CachedSourceFunc), while copies and moves of the
 * base class itself stay allowed.
 */
template <typename T, typename Base>
concept StrictSubclassOf =
    std::derived_from<std::remove_cvref_t<T>, Base> && !std::same_as<std::remove_cvref_t<T>, Base>;

/**
 * @brief Satisfied by the standard floating-point types @c float, @c double and @c long double
 * (cv-unqualified).
 *
 * Unlike @c std::floating_point, the extended floating-point types of C++23 (e.g.
 * @c std::float16_t) are excluded: not every @c <cmath> function is available for them.
 */
template <typename T>
concept StdFloatingPoint = OneOf<T, float, double, long double>;

/// @brief Satisfied if a @c const @p From can be converted to @p To by @c static_cast.
// NOLINT below: cpplint takes the requires-expression for a block followed by a redundant ';'.
template <typename From, typename To>
concept StaticCastableTo = requires(const From& f) { static_cast<To>(f); };  // NOLINT

/**
 * @brief Satisfied by contiguous sized ranges (e.g. @c std::vector, @c std::span, @c std::array,
 * @c base::DataVector) whose values are numbers convertible to @p To by @c static_cast.
 * @c bool is excluded: its values are not numbers.
 */
template <typename R, typename To>
concept ContiguousRangeCastableTo =
    std::ranges::contiguous_range<R> && std::ranges::sized_range<R> &&
    StaticCastableTo<std::ranges::range_value_t<R>, To> &&
    !std::same_as<std::ranges::range_value_t<R>, bool>;

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp
