#include <library/http/error.h>

#include <format>
#include <string_view>

namespace NHttp::NError {

    THttpError::THttpError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    THttpBadVersion::THttpBadVersion(THttpVersion bad_version)
        : THttpError(
            "HTTP BAD VERSION",
            std::format(
                "version {}.{}",
                bad_version.major,
                bad_version.minor
            )
        )
    {}

    THttpNotSetMethod::THttpNotSetMethod()
        : THttpError(
            "HTTP BAD REQUEST",
            "NOT SET METHOD"
        )
    {}

    THttpNotSetStatus::THttpNotSetStatus()
        : THttpError(
            "HTTP BAD RESPONSE",
            "NOT SET STATUS"
        )
    {}

    THttpBadTarget::THttpBadTarget(std::string_view bad_target)
        : THttpError(
            "HTTP BAD REQUEST",
            std::format(
                "Bad Target ( {} )",
                bad_target
            )
        )
    {}

    THttpBadHeaderName::THttpBadHeaderName(std::string_view bad_name)
        : THttpError(
            "HTTP BAD HEADER",
            std::format(
                "Bad Header Name ( {} )",
                bad_name
            )
        )
    {}

    THttpBadHeaderValue::THttpBadHeaderValue(std::string_view bad_value)
        : THttpError(
            "HTTP BAD HEADER",
            std::format(
                "Bad Header Value ( {} )",
                bad_value
            )
        )
    {}

    THttpConflictHeadersRequest::THttpConflictHeadersRequest() 
        : THttpError(
            "HTTP BAD REQUEST",
            "Conflict headers"
        )
    {}

    THttpConflictHeadersResponse::THttpConflictHeadersResponse() 
        : THttpError(
            "HTTP BAD RESPONSE",
            "Conflict headers"
        )
    {}

    THttpBadBody::THttpBadBody(size_t size)
        : THttpError(
            "HTTP BAD BODY",
            std::format(
                "Too large body ( size: {} )",
                size
            )
        )   
    {}

    THttpTooManyHeaders::THttpTooManyHeaders(size_t count) 
        : THttpError(
            "HTTP BAD HEADERS",
            std::format(
                "Too Many Headers ( the quantity reached: {} )",
                count
            )
        )
    {}

} // namespace NHttp::NError
