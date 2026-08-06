#pragma once 

#include <library/common/enum/enum.h>

#include <array>
#include <cstdint>

namespace NHttp {

    enum class EHttpRequestMethod : uint8_t {
        GET = 0,
        POST,
        PUT,
        PATCH,
        DELETE,
        HEAD,
        OPTIONS
    };

    constexpr auto get_enum_entries(std::type_identity<EHttpRequestMethod>) {
        return std::array{
            NEnum::TEnumEntry{EHttpRequestMethod::GET, "GET"},
            NEnum::TEnumEntry{EHttpRequestMethod::POST, "POST"},
            NEnum::TEnumEntry{EHttpRequestMethod::PUT, "PUT"},
            NEnum::TEnumEntry{EHttpRequestMethod::PATCH, "PATCH"},
            NEnum::TEnumEntry{EHttpRequestMethod::DELETE, "DELETE"},
            NEnum::TEnumEntry{EHttpRequestMethod::HEAD, "HEAD"},
            NEnum::TEnumEntry{EHttpRequestMethod::OPTIONS, "OPTIONS"}
        };
    }
    
} // namespace NHttp
