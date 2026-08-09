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
            "Http Bad Version",
            std::format(
                "version {}.{}",
                bad_version.major,
                bad_version.minor
            )
        )
    {}

} //namespace NHttp::NError
