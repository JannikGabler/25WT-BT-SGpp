// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * TODO: Document
 */
#pragma once

#include <concepts>

namespace sgpp {
namespace combigrid {

namespace misc {

/// TODO: Document
template <typename T, typename... Ts>
concept one_of = (std::same_as<T, Ts> || ...);

/// @brief Satisfied if @c std::remove_cvref_t<T> is derived from @p Base and is not @p Base.
template <typename T, typename Base>
concept StrictSubclassOf =
    std::derived_from<std::remove_cvref_t<T>, Base> && !std::same_as<std::remove_cvref_t<T>, Base>;

/// TODO: Document
template <typename T>
concept std_real_type = one_of<T, float, double, long double>;

/// TODO: Document
template <typename From, typename To>
concept static_castable_to = requires(const From& f) { static_cast<To>(f); };

}  // namespace misc

}  // namespace combigrid
}  // namespace sgpp