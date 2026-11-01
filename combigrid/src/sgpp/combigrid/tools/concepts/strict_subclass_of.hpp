// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file strict_subclass_of.hpp
 * @brief Concept "a (possibly cv/ref-qualified) type derived from @c Base, but not @c Base itself".
 */
#pragma once

#include <concepts>
#include <type_traits>

namespace sgpp {
namespace combigrid {
namespace tools {

/**
 * @brief Satisfied if @c std::remove_cvref_t<T> is derived from @p Base and is not @p Base.
 *
 * Used to delete the converting constructors and assignments of a base class that would slice a
 * subclass object (e.g. @c SourceFunc from a @c CachedSourceFunc), while copies and moves of the
 * base class itself stay allowed.
 */
template <typename T, typename Base>
concept StrictSubclassOf = std::derived_from<std::remove_cvref_t<T>, Base> &&
                           !std::same_as<std::remove_cvref_t<T>, Base>;

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
