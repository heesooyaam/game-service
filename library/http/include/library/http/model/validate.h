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
    inline constexpr size_t MAX_COUNTER_BYTES_REQUEST_LINE = 8192;
    inline constexpr size_t MAX_COUNTER_BYTES_HEADER = 8192;

    constexpr auto make_http_name_valid_chars() {
        std::array<bool, 256> valid_chars{};
        for (unsigned char c = 'a'; c <= 'z'; ++c) {
            valid_chars[c] = true;
        }
        for (unsigned char c = 'A'; c <= 'Z'; ++c) {
            valid_chars[c] = true;
        }
        for (unsigned char c = '0'; c <= '9'; ++c) {
            valid_chars[c] = true;
        }
        constexpr std::string_view extra_tchars = "!#$%&'*+-.^_`|~";
        for (unsigned char c : extra_tchars) {
            valid_chars[c] = true;
        }
        return valid_chars;
    }

    constexpr auto make_http_value_valid_chars() {
        std::array<bool, 256> valid_chars{};
        for (unsigned char c = 0x21; c <= 0x7E; ++c) {
            valid_chars[c] = true;
        }
        valid_chars[' ']  = true;
        valid_chars['\t'] = true;
        return valid_chars;
    }

    constexpr auto make_http_target_valid_chars() {
        std::array<bool, 256> valid_chars{};
        for (unsigned char c = 'a'; c <= 'z'; ++c) {
            valid_chars[c] = true;
        }
        for (unsigned char c = 'A'; c <= 'Z'; ++c) {
            valid_chars[c] = true;
        }
        for (unsigned char c = '0'; c <= '9'; ++c) {
            valid_chars[c] = true;
        }
        constexpr std::string_view uri_symbols = "-._~:/?@!$&'()*+,;=%";
        for (unsigned char c : uri_symbols) {
            valid_chars[c] = true;
        }
        return valid_chars;
    }

    constexpr auto make_http_protocol_valid_chars() {
        std::array<bool, 256> valid_chars{};
        for (size_t i = 0; i < 256; ++i) {   
            unsigned char c = static_cast<unsigned char>(i);
            
            bool is_alpha = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
            bool is_digit = (c >= '0' && c <= '9');
            bool is_special = (c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || 
                            c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' || 
                            c == '^' || c == '_' || c == '`' || c == '|' || c == '~');
                            
            valid_chars[i] = is_alpha || is_digit || is_special;
        }
        return valid_chars;
    }

    inline constexpr std::array<bool, 256> HTTP_NAME_VALID_CHARS = make_http_name_valid_chars();
    inline constexpr std::array<bool, 256> HTTP_VALUE_VALID_CHARS = make_http_value_valid_chars();
    inline constexpr std::array<bool, 256> HTTP_TARGET_VALID_CHARS = make_http_target_valid_chars();
    inline constexpr std::array<bool, 256> HTTP_PROTOCOL_VALID_CHARS = make_http_protocol_valid_chars();

    bool validate_version(const THttpVersion&) noexcept;
    bool validate_headers_count(size_t size) noexcept;
    bool validate_header_name_general(std::string_view);
    bool validate_header_value_general(std::string_view);
    bool validate_body(std::string_view);
    bool validate_body(size_t size) noexcept;

    bool validate_method(EHttpRequestMethod) noexcept;
    bool validate_origin_form_target(std::string_view);
    bool validate_headers_request(const THttpHeaders&, std::string_view body, EHttpRequestMethod);

    bool validate_status(EHttpResponseStatus) noexcept;
    bool validate_headers_response(const THttpHeaders&, std::string_view body, EHttpResponseStatus);

} // namespace NHttp::NModel
