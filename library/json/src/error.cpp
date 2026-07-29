#include <library/json/error.h>
#include <library/json/json_value.h>

#include <library/common/enum/enum.h>

#include <format>

namespace {

    std::string error_info(size_t pos, std::string_view msg) {
        return std::format("JSON parse error at byte {}; {}", pos, msg); 
    }

}

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

    TJsonParserErrorExtraData::TJsonParserErrorExtraData(size_t pos, std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Extra Data in document( {} )",
                error_info(pos, msg)
            )
        )
    {}

    TJsonParserErrorInvalidToken::TJsonParserErrorInvalidToken(size_t pos, std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Invalid Token( {} )",
                error_info(pos, msg)
            )
        )
    {}

    TJsonParserErrorMissingData::TJsonParserErrorMissingData(size_t pos, std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Missed data ( {} )",
                error_info(pos, msg)
            )
        )
    {}

    TJsonParserErrorUnexpectedEof::TJsonParserErrorUnexpectedEof(size_t pos, std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            std::format(
                "Unexpected Eof ( {} )",
                error_info(pos, msg)
            )
        )
    {}

    TJsonParserErrorDuplicateKey::TJsonParserErrorDuplicateKey(size_t pos, std::string_view msg)
        : TJsonParserError(
            "JSON PARSER ERROR",
            error_info(pos, msg)
        )
    {}

    TJsonSerializerError::TJsonSerializerError(std::string_view msg) 
        : TJsonError(
            "JSON SERIALIZER ERROR",
            msg
        )
    {}

} // namespace NJson::NError
