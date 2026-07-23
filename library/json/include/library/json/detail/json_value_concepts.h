#pragma once

#include <concepts>
#include <type_traits>

namespace NJson {

    template <typename T>
    concept CCharacter =
        std::same_as<std::remove_cvref_t<T>, char> ||
        std::same_as<std::remove_cvref_t<T>, signed char> ||
        std::same_as<std::remove_cvref_t<T>, unsigned char> ||
        std::same_as<std::remove_cvref_t<T>, wchar_t> ||
        std::same_as<std::remove_cvref_t<T>, char8_t> ||
        std::same_as<std::remove_cvref_t<T>, char16_t> ||
        std::same_as<std::remove_cvref_t<T>, char32_t>;

    template <typename T>
    concept CJsonInteger =
        std::integral<std::remove_cvref_t<T>> &&
        !std::same_as<std::remove_cvref_t<T>, bool> &&
        !CCharacter<T>;

    template <typename T>
    concept CJsonFloatingPoint =
        std::floating_point<std::remove_cvref_t<T>>;

    template <typename T>
    concept CJsonNumber = CJsonInteger<T> || CJsonFloatingPoint<T>;
    
} // namespace NJson
