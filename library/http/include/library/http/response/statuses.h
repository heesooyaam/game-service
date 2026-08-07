#pragma once 

#include <library/common/enum/enum.h>

#include <array>
#include <cstdint>

namespace NHttp {

    enum class EHttpResponseStatus : uint16_t {
        NOT_SET = 0,
        SWITCHING_PROTOCOLS = 101,
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501
    };

    constexpr auto get_enum_entries(std::type_identity<EHttpResponseStatus>) {
        return std::array{
            NEnum::TEnumEntry{EHttpResponseStatus::NOT_SET, "NOT_SET"},
            NEnum::TEnumEntry{EHttpResponseStatus::SWITCHING_PROTOCOLS, "SWITCHING PROTOCOLS"},
            NEnum::TEnumEntry{EHttpResponseStatus::OK, "OK"},
            NEnum::TEnumEntry{EHttpResponseStatus::CREATED, "CREATED"},
            NEnum::TEnumEntry{EHttpResponseStatus::NO_CONTENT, "NO CONTENT"},
            NEnum::TEnumEntry{EHttpResponseStatus::BAD_REQUEST, "BAD REQUEST"},
            NEnum::TEnumEntry{EHttpResponseStatus::UNAUTHORIZED, "UNAUTHORIZED"},
            NEnum::TEnumEntry{EHttpResponseStatus::FORBIDDEN, "FORBIDDEN"},
            NEnum::TEnumEntry{EHttpResponseStatus::NOT_FOUND, "NOT FOUND"},
            NEnum::TEnumEntry{EHttpResponseStatus::INTERNAL_SERVER_ERROR, "INTERNAL SERVER ERROR"},
            NEnum::TEnumEntry{EHttpResponseStatus::NOT_IMPLEMENTED, "NOT IMPLEMENTED"}
        };
    }
    
} // namespace NHttp
