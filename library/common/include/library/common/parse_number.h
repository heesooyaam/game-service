#pragma once

#include <cctype>
#include <charconv>
#include <optional>
#include <string_view>

namespace NCommon {

    template <typename T>
    std::optional<T> parse_number(std::string_view data) {
        T value{};
         const auto [ptr, error] = std::from_chars(
            data.data(),
            data.data() + data.size(),
            value
        );

        if (
            error != std::errc{} ||
            ptr != data.data() + data.size()
        ) {
            return std::nullopt;
        }
        
        return value;
    }

} // namespace NCommon
