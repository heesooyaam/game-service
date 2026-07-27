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
        explicit TJsonBadDoubleNumber(T value);
    };

    class TJsonIntegerOutOfRange : public TJsonError {
    public:
        template<CJsonInteger T> 
        explicit TJsonIntegerOutOfRange(T value);
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

    class TJsonParserError : public TJsonError {
    protected:
        explicit TJsonParserError(std::string_view, std::string_view);
    };

    class TJsonParserErrorEmptyDocument : public TJsonParserError {
    public:
        explicit TJsonParserErrorEmptyDocument();
    };

    class TJsonParserErrorExtraData : public TJsonParserError {
    public:
        explicit TJsonParserErrorExtraData(size_t pos, std::string_view);
    };

    class TJsonParserErrorInvalidToken : public TJsonParserError {
    public:
        explicit TJsonParserErrorInvalidToken(size_t pos, std::string_view);
    };

    class TJsonParserErrorMissingData : public TJsonParserError {
    public:
        explicit TJsonParserErrorMissingData(size_t pos, std::string_view);
    };

    class TJsonParserErrorUnexpectedEof : public TJsonParserError {
    public:
        explicit TJsonParserErrorUnexpectedEof(size_t pos, std::string_view);
    };

    class TJsonParserErrorDuplicateKey : public TJsonParserError {
    public:
        explicit TJsonParserErrorDuplicateKey(size_t pos, std::string_view);
    };

} // namespace NJson::NError
