#include <library/json/json_value.h>
#include <library/common/enum/enum.h>
#include <library/json/error.h>

#include <format>

namespace NJson::NError {

    TJsonError::TJsonError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    TJsonTypeError::TJsonTypeError(NJson::EJsonType expected, NJson::EJsonType actual)
        : TJsonError(
            "JSON TYPE ERROR",
            std::format(
                "Expected {}, but got {}", 
                NEnum::enum_to_string(expected).value(), 
                NEnum::enum_to_string(actual).value()
            )
        )
    {}

    TJsonArrayOutOfRange::TJsonArrayOutOfRange(size_t size, size_t index)
        : TJsonError(
            "JSON ARRAY OUT OF RANGE",
            std::format(
                "array size = {}, index = {}", 
                size, 
                index
            )
        )
    {}

    TJsonObjectOutOfRange::TJsonObjectOutOfRange(const std::string& key)
        : TJsonError(
            "JSON OBJECT OUT OF RANGE",
            key
        )
    {}

    TJsonBadPath::TJsonBadPath(std::string_view path, std::string_view msg)
        : TJsonError(
            "JSON BAD PATH",
            std::format(
                "path {} does not meet the requirements for the following reason: {}",
                path, 
                msg
            )
        )
    {}

    TJsonOperationError::TJsonOperationError(NJson::EJsonType actual)
        : TJsonError(
            "JSON OPERATION ERROR",
            std::format(
                "not supported operation for type {}", 
                NEnum::enum_to_string(actual).value()
            )
        )
    {}

    TJsonParserError::TJsonParserError(std::string_view label, std::string_view msg)
        : TJsonError(label, msg)
    {}

    TJsonParserErrorEmptyDocument::TJsonParserErrorEmptyDocument() 
        : TJsonParserError(
            "JSON PARSER ERROR",
            "Empty document"
        )
    {}  

    TJsonParserErrorExtraData::TJsonParserErrorExtraData(std::string_view extra_data)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Extra Data in document( {} )",
                extra_data
            )
        )
    {}

    TJsonParserErrorInvalidToken::TJsonParserErrorInvalidToken(std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Invalid Token( {} )",
                msg
            )
        )
    {}

    TJsonParserErrorMissingData::TJsonParserErrorMissingData(std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Missed data ( {} )",
                msg
            )
        )
    {}

    TJsonParserErrorUnexpectedEof::TJsonParserErrorUnexpectedEof(std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Unexpected Eof ( {} )",
                msg
            )
        )
    {}

    TJsonParserErrorDuplicateKey::TJsonParserErrorDuplicateKey(std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            msg
        )
    {}

} // namespace NJson::NError
