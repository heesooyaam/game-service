#include <library/json/json_value.h>
#include <library/common/enum/enum.h>
#include <library/json/error.h>

#include <format>

namespace NJson::NError {

    TJsonError::TJsonError() 
        : std::runtime_error("[JSON ERROR]")
    {}

    TJsonError::TJsonError(const std::string& msg)
        : std::runtime_error(
            std::format(
                "[JSON ERROR]: {}", 
                msg
            )
        )
    {}

    TJsonError::TJsonError(const char* msg)
        : std::runtime_error(
            std::format(
                "[JSON ERROR]: {}", 
                msg
            )
        )
    {}

    TJsonError::TJsonError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    TJsonTypeError::TJsonTypeError()
        :  TJsonError("JSON TYPE ERROR", "UNKNOWN")
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

    TJsonTypeError::TJsonTypeError(const std::string& msg)
        : TJsonError(
            "JSON TYPE ERROR",
            msg
        )
    {}

    TJsonTypeError::TJsonTypeError(const char* msg)
        : TJsonError(
            "JSON TYPE ERROR",
            msg
        )
    {}

    TJsonArrayOutOfRange::TJsonArrayOutOfRange()
        : TJsonError("JSON ARRAY OUT OF RANGE", "UNKNOWN")
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

    TJsonObjectOutOfRange::TJsonObjectOutOfRange()
        : TJsonError("JSON OBJECT OUT OF RANGE", "UNKNOWN")
    {}

    TJsonObjectOutOfRange::TJsonObjectOutOfRange(const std::string& key)
        : TJsonError(
            "JSON OBJECT OUT OF RANGE",
            key
        )
    {}

    TJsonBadPath::TJsonBadPath()
        : TJsonError("JSON BAD PATH", "UNKNOWN")
    {}

    TJsonBadPath::TJsonBadPath(const std::string& msg)
        : TJsonError(
            "JSON BAD PATH",
            msg
        )
    {}

    TJsonBadPath::TJsonBadPath(const char* msg)
        : TJsonError(
            "JSON BAD PATH",
            msg
        )
    {}

    TJsonBadDoubleNumber::TJsonBadDoubleNumber()
        : TJsonError("JSON DOUBLE BAD NUMBER", "UNKNOWN")
    {}

    TJsonBadIntegerNumber::TJsonBadIntegerNumber()
        : TJsonError("JSON INTEGER BAD NUMBER", "UNKNOWN")
    {}

} // namespace NJson::NError
