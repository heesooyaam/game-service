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

    TJsonTypeError::TJsonTypeError()
        :  TJsonError("[JSON TYPE ERROR]")
    {}

    TJsonTypeError::TJsonTypeError(NJson::EJsonType expected, NJson::EJsonType actual)
        : TJsonError(
            std::format(
                "[JSON TYPE ERROR]: Expected {}, but got {}", 
                NEnum::enum_to_string(expected).value(), 
                NEnum::enum_to_string(actual).value()
            )
        )
    {}

    TJsonTypeError::TJsonTypeError(const std::string& msg)
        : TJsonError(
            std::format(
                "[JSON TYPE ERROR]: {}", 
                msg
            )
        )
    {}

    TJsonTypeError::TJsonTypeError(const char* msg)
        : TJsonError(
            std::format(
                "[JSON TYPE ERROR]: {}", 
                msg
            )
        )
    {}

    TJsonArrayOutOfRange::TJsonArrayOutOfRange()
        : TJsonError("[JSON ARRAY OUT OF RANGE]")
    {}

    TJsonArrayOutOfRange::TJsonArrayOutOfRange(size_t size, size_t index)
        : TJsonError(
            std::format(
                "[JSON ARRAY OUT OF RANGE]: array size = {}, index = {}", 
                size, 
                index
            )
        )
    {}

    TJsonObjectOutOfRange::TJsonObjectOutOfRange()
        : TJsonError("[JSON OBJECT OUT OF RANGE]")
    {}

    TJsonObjectOutOfRange::TJsonObjectOutOfRange(const std::string& key)
        : TJsonError(
            std::format(
                "[JSON OBJECT OUT OF RANGE]: {}", 
                key
            )
        )
    {}

    TJsonBadPath::TJsonBadPath()
        : TJsonError("[JSON BAD PATH]")
    {}

    TJsonBadPath::TJsonBadPath(const std::string& msg)
        : TJsonError(
            std::format(
                "[JSON BAD PATH]: {}", 
                msg
            )
        )
    {}

    TJsonBadPath::TJsonBadPath(const char* msg)
        : TJsonError(
            std::format(
                "[JSON BAD PATH]: {}", 
                msg
            )
        )
    {}

    TJsonBadDoubleNumber::TJsonBadDoubleNumber()
        : TJsonError("[JSON DOUBLE BAD NUMBER]")
    {}

    TJsonBadIntegerNumber::TJsonBadIntegerNumber()
        : TJsonError("[JSON INTEGER BAD NUMBER]")
    {}

} // namespace NJson::NError
