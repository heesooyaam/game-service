#include <library/json/error.h>
#include <library/json/json_constants.h>
#include <library/json/json_parser.h>
#include <library/json/json_value.h>
#include <library/common/parse_number.h>
#include <library/json/json_constants.h>

#include <array>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <string_view>

namespace NJson::NDetail {
    bool is_json_whitespace(char c) noexcept {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }
}

namespace NJson {

    TJsonValue parse(std::string_view data) {
        return TJsonParser(data).parse();
    }

    constexpr auto delimiters = std::array{
        NConstants::OPEN_OBJECT,
        NConstants::CLOSE_OBJECT,
        NConstants::OPEN_ARRAY,
        NConstants::CLOSE_ARRAY,
        NConstants::OPEN_STRING,
        NConstants::CLOSE_STRING,
        NConstants::COMMA,
        NConstants::COLON,
    };

    TJsonParser::TJsonParser(std::string_view data)
        : data_(data)
        , pos_(0)
    {}

    TJsonValue TJsonParser::parse() {
        skip_space();

        if (is_eof()) {
            throw NError::TJsonParserErrorEmptyDocument();
        }

        auto json = parse_value();
        skip_space();

        if (!is_eof()) {
            throw NError::TJsonParserErrorExtraData(pos_, data_.substr(pos_)); 
        }

        return json;
    }

    char TJsonParser::peek() const noexcept {
        return data_[pos_];
    }

    void TJsonParser::next() noexcept {
        assert(!is_eof());
        pos_++;
        skip_space();
    }

    void TJsonParser::skip_space() noexcept {
        while (!is_eof() && NDetail::is_json_whitespace(peek())) {
            ++pos_;
        }
    }

    bool TJsonParser::is_eof() const noexcept {
        return pos_ == data_.size();
    }

    std::string_view TJsonParser::substr_to_delim() noexcept {
        size_t start = pos_;
        while (
            !is_eof() 
            && std::find(delimiters.begin(), delimiters.end(), peek()) == delimiters.end()
            && !NDetail::is_json_whitespace(peek())
        ) {
            ++pos_;
        }
        return data_.substr(start, pos_ - start);
    }

    TJsonValue TJsonParser::parse_value() {
        assert(!is_eof() && !NDetail::is_json_whitespace(peek()));
        switch (peek()) {
            case NConstants::OPEN_STRING: {
                return parse_string();
            }
            case NConstants::OPEN_ARRAY: {
                return parse_array();
            }
            case NConstants::OPEN_OBJECT: {
                return parse_object();
            }
            default: {
                return parse_simple_value();
            }
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
            if (substr.size() > 1) {
                if (substr[0] == '0' && std::isdigit(static_cast<unsigned char>(substr[1]))) {
                    bad_number = true;
                }
                
                if (substr.size() > 2 && substr[0] == '-' && substr[1] == '0' && std::isdigit(static_cast<unsigned char>(substr[2]))) {
                    bad_number = true;
                }
            }
            
            auto opt_integer = NCommon::parse_number<TInteger>(substr);
            if (opt_integer.has_value()) {
                if (bad_number) {
                    throw NError::TJsonParserErrorInvalidToken(pos_, "leading zeros are not allowed in parsing number");
                }
                return TJsonValue(opt_integer.value());
            }
                
            auto opt_double = NCommon::parse_number<TDouble>(substr);
            if (opt_double.has_value()) {
                if (bad_number) {
                    throw NError::TJsonParserErrorInvalidToken(pos_, "leading zeros are not allowed in parsing number");
                }
                return TJsonValue(opt_double.value());
            }
        }

        throw NError::TJsonParserErrorInvalidToken(pos_ - substr.size(), substr);
    }


    TJsonValue TJsonParser::parse_string() {
        if (peek() != NConstants::OPEN_STRING) {
            throw NError::TJsonParserErrorMissingData(pos_, std::format("parsing string, miss {}", NConstants::OPEN_STRING));
        }

        TString str;
        ++pos_;
        for (; !is_eof() && peek() != NConstants::CLOSE_STRING; ++pos_) {
            if (peek() == '\\') {
                ++pos_;
                if (is_eof()) {
                    throw NError::TJsonParserErrorInvalidToken(pos_, std::string("parsing string, one \\"));
                }

                char escaped_char = peek();
                switch (escaped_char) {
                    case '"': {
                        str.push_back('"'); 
                        break;  
                    }
                    case '\\': {
                        str.push_back('\\'); 
                        break; 
                    }
                    case '/': {
                        str.push_back('/'); 
                        break;  
                    }
                    case 'b': {
                        str.push_back('\b'); 
                        break; 
                    }
                    case 'f': {
                        str.push_back('\f'); 
                        break; 
                    }
                    case 'n': {
                        str.push_back('\n'); 
                        break; 
                    }
                    case 'r': {
                        str.push_back('\r'); 
                        break; 
                    }
                    case 't': {
                        str.push_back('\t'); 
                        break; 
                    }
                    default: {
                        throw NError::TJsonParserErrorInvalidToken(pos_, std::string("parsing string, bad \\"));
                    }
                }
            } else {    
                const auto character = static_cast<unsigned char>(peek());

                if (character < 0x20) {
                    throw NError::TJsonParserErrorInvalidToken(pos_,"unescaped control character in string");
                }

                str.push_back(peek());
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof(pos_, "parsing string");
        }

        ++pos_;
        return TJsonValue(std::move(str));
    }

    TJsonValue TJsonParser::parse_array() {
        assert(peek() == NConstants::OPEN_ARRAY);
        TArray array;
        next();

        while (!is_eof() && peek() != NConstants::CLOSE_ARRAY) {
            array.push_back(parse_value());
            skip_space();
            if (!is_eof()) {
                if (peek() == NConstants::COMMA) {
                    next();
                    if (!is_eof() && peek() == NConstants::CLOSE_ARRAY) {
                        throw NError::TJsonParserErrorInvalidToken(pos_, std::string("parsing array, bad comma"));
                    }
                } else {
                    if (peek() != NConstants::CLOSE_ARRAY) {
                        throw NError::TJsonParserErrorMissingData(pos_, std::format("parsing array, miss {}", NConstants::COMMA));
                    }
                }
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof(pos_, "parsing array");
        }

        ++pos_;
        return TJsonValue(std::move(array));
    }

    TJsonValue TJsonParser::parse_object() {
        assert(peek() == NConstants::OPEN_OBJECT);
        TObject object;
        next();

        while (!is_eof() && peek() != NConstants::CLOSE_OBJECT) {
            auto json_key = parse_string();
            skip_space();
            if (!is_eof()) {
                if (peek() != NConstants::COLON) {
                    throw NError::TJsonParserErrorMissingData(pos_, std::format("parsing object, miss {}", NConstants::COLON));
                }
                next();
                if (!is_eof()) {
                    auto json_value = parse_value();

                    auto [iterator, inserted] = object.try_emplace(
                        std::move(json_key.get_string()),
                        std::move(json_value)
                    );

                    if (!inserted) {
                        throw NError::TJsonParserErrorDuplicateKey(pos_, "Duplicate object key");
                    }

                    skip_space();
                    if (!is_eof()) {
                        if (peek() == NConstants::COMMA) {
                            next();
                            if (!is_eof() && peek() == NConstants::CLOSE_OBJECT) {
                                throw NError::TJsonParserErrorInvalidToken(pos_, std::string("parsing object, bad comma"));
                            }
                        } else {
                            if (peek() != NConstants::CLOSE_OBJECT) {
                                throw NError::TJsonParserErrorMissingData(pos_, std::format("parsing object, miss {}", NConstants::COMMA));
                            }
                        }
                    }
                }
            }
        }

        if (is_eof()) {
            throw NError::TJsonParserErrorUnexpectedEof(pos_,"parsing object");
        }

        ++pos_;
        return TJsonValue(std::move(object));
    }


} //namespace NJson
