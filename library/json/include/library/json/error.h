#pragma once

#include <library/json/detail/json_value_concepts.h>
#include <library/json/json_types.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <format>

namespace NJson::NError {

    class TJsonError : public std::runtime_error {
    protected:
        explicit TJsonError(std::string_view, std::string_view);
    };

    class TJsonTypeError : public TJsonError {
    public:
        explicit TJsonTypeError(NJson::EJsonType expected, NJson::EJsonType actual);
    };

    class TJsonArrayOutOfRange : public TJsonError {
    public:
        explicit TJsonArrayOutOfRange(size_t, size_t);
    };

    class TJsonObjectOutOfRange : public TJsonError {
    public:        
        explicit TJsonObjectOutOfRange(const std::string&);
    };

    class TJsonBadPath : public TJsonError {
    public:
        explicit TJsonBadPath(std::string_view, std::string_view);
    };

    class TJsonOperationError : public TJsonError {
    public: 
        explicit TJsonOperationError(NJson::EJsonType actual);
    };

    class TJsonBadDoubleNumber : public TJsonError {
    public:
        template<CJsonFloatingPoint T> 
        TJsonBadDoubleNumber(T value);
    };

    class TJsonIntegerOutOfRange : public TJsonError {
    public:
        template<CJsonInteger T> 
        TJsonIntegerOutOfRange(T value);
    };

    template<CJsonFloatingPoint T> 
    TJsonBadDoubleNumber::TJsonBadDoubleNumber(T value) 
        : TJsonError(
            "JSON DOUBLE BAD NUMBER", 
            std::format(
                "{} does not meet the Json Double Number requirements",
                value
            )
        )
    {}

    template<CJsonInteger T> 
    TJsonIntegerOutOfRange::TJsonIntegerOutOfRange(T value) 
        : TJsonError(
            "JSON INTEGER OUT OF RANGE", 
            std::format(
                "integer {} is out of range",
                value
            )
        )
    {}

    class TJsonParserErrorEmptyDocument : public TJsonError {
    public:
        TJsonParserErrorEmptyDocument();
    };

    class TJsonParserErrorExtraData : public TJsonError {
    public:
        TJsonParserErrorExtraData(std::string_view);
    };

    class TJsonParserErrorInvalidToken : public TJsonError {
    public:
        TJsonParserErrorInvalidToken(std::string_view);
        TJsonParserErrorInvalidToken(std::string_view label, std::string_view msg);
    };

    class TJsonParserErrorMissingData : public TJsonError {
    public:
        TJsonParserErrorMissingData(std::string_view label, const char symbol);
    };

    class TJsonParserErrorUnexpectedEof : public TJsonError {
    public:
        TJsonParserErrorUnexpectedEof(std::string_view);
    };

} // namespace NJson::NError
