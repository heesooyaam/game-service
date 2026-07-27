#pragma once

#include <library/json/json_types.h>

#include <string_view>

namespace NJson {

    TJsonValue parse(std::string_view data);

    class TJsonParser {
    private:
        std::string_view data_;
        size_t pos_;
    public:
        explicit TJsonParser(std::string_view);

        TJsonParser(const TJsonParser&) = delete;
        TJsonParser& operator=(const TJsonParser&) = delete;

        TJsonParser(TJsonParser&&) = delete;
        TJsonParser& operator=(TJsonParser&&) = delete;
        
        TJsonValue parse();
    private:
        char peek() const noexcept;
        void next() noexcept;
        void skip_space() noexcept;
        bool is_eof() const noexcept;

        std::string_view substr_to_delim() noexcept;

        TJsonValue parse_value();
        TJsonValue parse_simple_value();

        TJsonValue parse_string();
        TJsonValue parse_array();
        TJsonValue parse_object();
    };

} //namespace NJson
