#pragma once

#include <library/json/json_types.h>

#include <string_view>

namespace NJson {

    class TJsonParser {
    private:
        std::string_view data_;
        size_t pos;
    public:
        explicit TJsonParser(std::string_view);

        TJsonParser(const TJsonParser&) = delete;
        TJsonParser& operator=(const TJsonParser&) = delete;

        TJsonParser(TJsonParser&&) = delete;
        TJsonParser& operator=(TJsonParser&&) = delete;
        
        TJsonValue parse();
    private:
        char peek() const;
        void next();
        void skip_space();
        bool is_eof() const;

        std::string_view substr_to_delim();

        TJsonValue parse_value();
        TJsonValue parse_simple_value();

        TJsonValue parse_string();
        TJsonValue parse_array();
        TJsonValue parse_object();
    };

} //namespace NJson
