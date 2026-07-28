#pragma once

#include <cctype>
#include <charconv>
#include <optional>
#include <string>

namespace NCommon {

    std::optional<std::string> show_to_chars(auto... format_args) {
        const size_t buf_size = 20;
        char buf[buf_size]{};
        std::to_chars_result result = std::to_chars(buf, buf + buf_size, format_args...);

        if (result.ec != std::errc()) {
            return std::nullopt;
        }
        
        return std::string(buf, result.ptr - buf);
    }

} //namespace NCommon
