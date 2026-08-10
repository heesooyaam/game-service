#pragma once

#include <library/http/headers.h>
#include <library/http/request/methods.h>
#include <library/http/response/statuses.h>
#include <library/http/version.h>

#include <array>
#include <string_view>

namespace NHttp::NModel {

    inline constexpr size_t MAX_COUNTER_HEADERS = 100;
    inline constexpr size_t MAX_COUNTER_BYTES_BODY = 16384;

    constexpr auto make_http_char_name_lut() {
        std::array<bool, 256> lut{};
        for (unsigned char c = 'a'; c <= 'z'; ++c) lut[c] = true;
        for (unsigned char c = 'A'; c <= 'Z'; ++c) lut[c] = true;
        for (unsigned char c = '0'; c <= '9'; ++c) lut[c] = true;
        constexpr std::string_view extra_tchars = "!#$%&'*+-.^_`|~";
        for (unsigned char c : extra_tchars) {
            lut[c] = true;
        }
        return lut;
    }

    constexpr auto make_http_char_value_lut() {
        std::array<bool, 256> lut{};
        for (unsigned char c = 0x21; c <= 0x7E; ++c) {
            lut[c] = true;
        }
        lut[' ']  = true;
        lut['\t'] = true;
        return lut;
    }

    constexpr auto make_http_char_target_lut() {
        std::array<bool, 256> lut{};
        for (unsigned char c = 'a'; c <= 'z'; ++c) lut[c] = true;
        for (unsigned char c = 'A'; c <= 'Z'; ++c) lut[c] = true;
        for (unsigned char c = '0'; c <= '9'; ++c) lut[c] = true;
        constexpr std::string_view uri_symbols = "-._~:/?#[]@!$&'()*+,;=%";
        for (unsigned char c : uri_symbols) {
            lut[c] = true;
        }
        return lut;
    }

    inline constexpr std::array<bool, 256> HTTP_CHAR_NAME_LUT = make_http_char_name_lut();
    inline constexpr std::array<bool, 256> HTTP_CHAR_VALUE_LUT = make_http_char_value_lut();
    inline constexpr std::array<bool, 256> HTTP_CHAR_TARGET_LUT = make_http_char_target_lut();

    bool validate_version(const THttpVersion&);
    bool validate_headers_count(size_t size);
    bool validate_header_name_general(std::string_view);
    bool validate_header_value_general(std::string_view);
    bool validate_body(std::string_view);

    bool validate_method(EHttpRequestMethod);
    bool validate_target(std::string_view);
    bool validate_headers_request(const THttpHeaders&, std::string_view body, EHttpRequestMethod);

    bool validate_status(EHttpResponseStatus);
    bool validate_headers_response(const THttpHeaders&, std::string_view body, EHttpResponseStatus);

} //namespace NHttp::NModel
