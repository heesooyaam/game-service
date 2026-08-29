#pragma once

#include <string_view>
#include <vector>

namespace NCommon {

    constexpr std::string_view trim_ows(std::string_view sv) noexcept {
        while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) {
            sv.remove_prefix(1);
        }
        while (!sv.empty() && (sv.back() == ' ' || sv.back() == '\t')) {
            sv.remove_suffix(1);
        }
        return sv;
    }

    std::vector<std::string_view> split(std::string_view data, char separator);

} // namespace NCommon
