#pragma once

#include <optional>
#include <string_view>
#include <type_traits>

namespace NEnum {

template <typename T>
struct TEnumEntry {
    T value;
    std::string_view name;
};

template <typename T>
concept CDescribedEnum =
    std::is_enum_v<T> &&
    requires {
        get_enum_entries(std::type_identity<T>{});
    };

template <CDescribedEnum T>
constexpr std::optional<std::string_view> enum_to_string(T value) noexcept {
    for (const auto& entry : get_enum_entries(std::type_identity<T>{})) {
        if (entry.value == value) {
            return entry.name;
        }
    }

    return std::nullopt;
}

template <CDescribedEnum T>
constexpr std::optional<T> enum_from_string(std::string_view name) noexcept {
    for (const auto& entry : get_enum_entries(std::type_identity<T>{})) {
        if (entry.Name == name) {
            return entry.Value;
        }
    }

    return std::nullopt;
}

} // namespace NEnum
