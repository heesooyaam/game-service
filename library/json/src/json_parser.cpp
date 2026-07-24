#include <library/json/json_types.h>
#include <library/json/json_parser.h>
#include <library/json/json_value.h>
#include <library/common/parse_number.h>

#include <array>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <string_view>

namespace  {
    bool is_json_space(char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }
}

namespace NJson {

    constexpr char OPEN_OBJECT = '{';
    constexpr char CLOSE_OBJECT = '}';
    constexpr char OPEN_ARRAY = '[';
    constexpr char CLOSE_ARRAY = ']';
    constexpr char OPEN_STRING = '"';
    constexpr char CLOSE_STRING = '"';
    constexpr char COMMA = ',';
    constexpr char COLON = ':';

    constexpr auto delimiters = std::array{
        OPEN_OBJECT,
        CLOSE_OBJECT,
        OPEN_ARRAY,
        CLOSE_ARRAY,
        OPEN_STRING,
        CLOSE_STRING,
        COMMA,
        COLON,
    };

    TJsonParser::TJsonParser(std::string_view data)
        : data_(data)
        , pos(0)
    {}

    TJsonValue TJsonParser::parse() {
        skip_space();

        if (is_eof()) {
            throw std::runtime_error("clear parse data");
        }

        auto json = parse_value();
        skip_space();

        
        if (!is_eof()) {
            throw std::runtime_error(std::format("unknown symbol left {}", data_.substr(pos))); 
        }

        return json;
    }

    char TJsonParser::peek() const {
        return data_[pos];
    }

    void TJsonParser::next() {
        if (is_eof()) {
            return;
        }
        pos++;
        skip_space();
    }

    void TJsonParser::skip_space() {
        while (!is_eof() && is_json_space(peek())) {
            ++pos;
        }
    }

    bool TJsonParser::is_eof() const {
        return pos == data_.size();
    }

    std::string_view TJsonParser::substr_to_delim() {
        size_t start = pos;
        while (!is_eof() 
            && std::find(delimiters.begin(), delimiters.end(), peek()) == delimiters.end()
            && !is_json_space(peek())
        ) {
            ++pos;
        }
        return data_.substr(start, pos - start);
    }

    TJsonValue TJsonParser::parse_value() {
        assert(!is_eof() && peek() != ' ');
        switch (peek()) {
            case OPEN_STRING: return parse_string();
            case OPEN_ARRAY: return parse_array();
            case OPEN_OBJECT: return parse_object();
            default: return parse_simple_value();
        }   
    }

    TJsonValue TJsonParser::parse_simple_value() {
        auto substr = substr_to_delim();

        if (substr == "null") {
            return TJsonValue();
        } else if (substr == "true") {
            return TJsonValue(true);
        } else if (substr == "false") {
            return TJsonValue(false);
        } else {
            auto opt_integer = NCommon::parse_number<TInteger>(substr);
            if (opt_integer.has_value()) {
                return TJsonValue(opt_integer.value());
            }

            auto opt_double = NCommon::parse_number<TDouble>(substr);
            if (opt_double.has_value()) {
                return TJsonValue(opt_double.value());
            }
        }

        throw std::runtime_error(std::format("unknown type {}", substr));
    }


    TJsonValue TJsonParser::parse_string() {
        if (peek() != OPEN_STRING) {
            throw std::runtime_error("no '\"' to parse string");
        }

        size_t start = pos;
        next();
        for (; !is_eof() && peek() != CLOSE_STRING; next()) {}

        if (is_eof()) {
            throw std::runtime_error("no '\"' to close parse string");
        }

        ++pos;
        return TJsonValue(TString(data_.substr(start + 1, pos - start - 2)));
    }

    TJsonValue TJsonParser::parse_array() {
        assert(peek() == OPEN_ARRAY);
        TArray array;
        next();

        while (!is_eof() && peek() != CLOSE_ARRAY) {

            array.push_back(parse_value());
            skip_space();

            if (!is_eof()) {
                
                if (peek() == COMMA) {
                    next();
                    if (!is_eof() && peek() == CLOSE_ARRAY) {
                        throw std::runtime_error("bad comma (array)");
                    }
                } else {

                    if (peek() != CLOSE_ARRAY) {
                        throw std::runtime_error("no comma no ] (array)");
                    }

                }

            }

        }

        if (is_eof()) {
            throw std::runtime_error("no ']' to close parse array");
        }

        ++pos;
        return TJsonValue(std::move(array));
    }

    TJsonValue TJsonParser::parse_object() {
        assert(peek() == OPEN_OBJECT);
        TObject object;
        next();

        while (!is_eof() && peek() != CLOSE_OBJECT) {
            auto json_key = parse_string();
            skip_space();

            if (!is_eof()) {

                if (peek() != COLON) {
                    throw std::runtime_error("no : after key (object)");
                }

                next();
                if (!is_eof()) {

                    auto json_value = parse_value();
                    object.insert({json_key.get_string(), json_value});
                    
                    skip_space();

                    if (!is_eof()) {
                        
                        if (peek() == COMMA) {
                            next();
                            if (!is_eof() && peek() == CLOSE_OBJECT) {
                                throw std::runtime_error("bad comma (object)");
                            }
                        } else {

                            if (peek() != CLOSE_OBJECT) {
                                throw std::runtime_error("no comma (object)");
                            }

                        }

                    }


                }


            }

        }

        if (is_eof()) {
            throw std::runtime_error("cant close object");
        }

        ++pos;
        return TJsonValue(std::move(object));
    }


} //namespace NJson
