#include <library/json/error.h>
#include <library/json/json_parser.h>
#include <library/json/json_value.h>
#include <library/common/parse_number.h>

#include <array>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <string_view>

namespace {
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
            throw NError::TJsonParserErrorEmptyDocument();
        }

        auto json = parse_value();
        skip_space();

        if (!is_eof()) {
            throw NError::TJsonParserErrorExtraData(data_.substr(pos)); 
        }

        return json;
    }

    char TJsonParser::peek() const {
        return data_[pos];
    }

    void TJsonParser::next() {
        assert(!is_eof());
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
        while (
            !is_eof() 
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

            bool bad_number = false;
            {
                if (substr.size() > 1) {
                    if (substr[0] == '0' && std::isdigit(substr[1])) {
                        bad_number = true;
                    }
                    
                    if (substr.size() > 2 && substr[0] == '-' && substr[1] == '0' && std::isdigit(substr[2])) {
                        bad_number = true;
                    }
                }
            }
            
            
            auto opt_integer = NCommon::parse_number<TInteger>(substr);
            if (opt_integer.has_value()) {
                if (bad_number) {
                    throw NError::TJsonParserErrorInvalidToken("leading zeros are not allowed");
                }
                return TJsonValue(opt_integer.value());
            }
                
            auto opt_double = NCommon::parse_number<TDouble>(substr);
            if (opt_double.has_value()) {
                if (bad_number) {
                    throw NError::TJsonParserErrorInvalidToken("leading zeros are not allowed");
                }
                return TJsonValue(opt_double.value());
            }
        }

        throw NError::TJsonParserErrorInvalidToken(substr);
    }


    TJsonValue TJsonParser::parse_string() {
        if (peek() != OPEN_STRING) {
            throw NError::TJsonParserErrorMissingData("parsing string", OPEN_STRING);
        }

        TString str;
        ++pos;
        for (; !is_eof() && peek() != CLOSE_STRING; ++pos) {
            if (peek() == '\\') {

                ++pos;
                if (is_eof()) {
                    throw NError::TJsonParserErrorInvalidToken("parsing string", "one \\");
                }

                char escaped_char = peek();
                switch (escaped_char) {
                    case '"':  
                        str.push_back('"'); 
                        break;  
                    case '\\': 
                        str.push_back('\\'); 
                        break; 
                    case '/':  
                        str.push_back('/'); 
                        break;  
                    case 'b':  
                        str.push_back('\b'); 
                        break; 
                    case 'f':  
                        str.push_back('\f'); 
                        break; 
                    case 'n':  
                        str.push_back('\n'); 
                        break; 
                    case 'r':  
                        str.push_back('\r'); 
                        break; 
                    case 't':  
                        str.push_back('\t'); 
                        break; 
                    default:
                        throw NError::TJsonParserErrorInvalidToken("parsing string", "bad \\");
                    }

            } else {
                str.push_back(peek());
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof("parsing string");
        }

        ++pos;
        return TJsonValue(std::move(str));
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
                        throw NError::TJsonParserErrorInvalidToken("parsing array", "bad comma");
                    }
                } else {
                    if (peek() != CLOSE_ARRAY) {
                        throw NError::TJsonParserErrorMissingData("parsing array", COMMA);
                    }
                }
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof("parsing array");
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
                    throw NError::TJsonParserErrorMissingData("parsing object", COLON);
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
                                throw NError::TJsonParserErrorInvalidToken("parsing object", "bad comma");
                            }
                        } else {
                            if (peek() != CLOSE_OBJECT) {
                                throw NError::TJsonParserErrorMissingData("parsing object", COMMA);
                            }
                        }
                    }
                }
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof("parsing object");
        }

        ++pos;
        return TJsonValue(std::move(object));
    }


} //namespace NJson
