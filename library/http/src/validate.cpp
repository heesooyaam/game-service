#include <library/http/headers.h>
#include <library/http/request/methods.h>
#include <library/http/response/statuses.h>
#include <library/http/model/validate.h>
#include <library/http/model/date.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <optional>
#include <string_view>

namespace {

    constexpr bool is_digit(char c) {
        const auto uc = static_cast<unsigned char>(c);
        return uc >= '0' && uc <= '9';
    }

    constexpr bool is_hex_digit(char c) {
        const auto uc = static_cast<unsigned char>(c);
        return is_digit(c) || (uc >= 'a' && uc <= 'f') || (uc >= 'A' && uc <= 'F');
    }

    bool validate_content_length_transfer_encoding(const NHttp::THttpHeaders& headers, std::string_view body) {
        auto transfer_encoding_value_opt = headers.get_value("Transfer-Encoding");
        if (transfer_encoding_value_opt.has_value()) {
            return false;
        }

        auto content_length_value_opt = headers.get_value("Content-Length");
        if (content_length_value_opt.has_value()) {
            auto content_length_values = headers.get_values("Content-Length");
            if (content_length_values.size() != 1) {
                return false;
            }
            auto content_length_value = content_length_value_opt.value();
            if (!std::all_of(
                content_length_value.begin(),
                content_length_value.end(), 
                [](char c) { 
                    return is_digit(c);
                })
            ) {
                return false;
            }
            
            if (std::to_string(body.size()) != content_length_value) {
                return false;
            }
        } else if (!body.empty()) {
            return false;
        }
        return true;
    }
}

namespace NHttp::NModel {

    bool validate_version(const THttpVersion& version) noexcept {
        return version == THttpVersion(1, 1);
    }

    bool validate_headers_count(size_t size) noexcept {
        if (size > MAX_COUNTER_HEADERS) {
            return false;
        }
        return true;
    }

    bool validate_header_name_general(std::string_view name) {
        if (name.empty()) {
            return false;
        }
        return std::all_of(name.begin(), name.end(), [](char c) noexcept {
            return HTTP_CHAR_NAME_VALID_CHARS[static_cast<unsigned char>(c)];
        });
    }

    bool validate_header_value_general(std::string_view value) {
        return std::all_of(value.begin(), value.end(), [](char c) noexcept {
            return HTTP_CHAR_VALUE_VALID_CHARS[static_cast<unsigned char>(c)];
        });
    }

    bool validate_body(std::string_view body) {
        return body.size() <= MAX_COUNTER_BYTES_BODY;
    }

    bool validate_method(EHttpRequestMethod method) noexcept {
        return
            method != EHttpRequestMethod::NOT_SET &&
            NEnum::enum_to_string(method).has_value();
    }

    bool validate_origin_form_target(std::string_view target) {
        if (target.empty() || target.front() != '/') {
            return false;   
        }

        for (size_t i = 0; i < target.size(); ++i) {
            const unsigned char c = static_cast<unsigned char>(target[i]);
            if (!HTTP_CHAR_TARGET_VALID_CHARS[c]) {
                return false;
            }

            if (c == '%') {
                if (i + 2 >= target.size() || 
                !is_hex_digit(target[i + 1]) || 
                !is_hex_digit(target[i + 2])) {
                    return false; 
                }
                i += 2;
            }
        }

        return true;
    }

    bool validate_headers_request(const THttpHeaders& headers, std::string_view body, EHttpRequestMethod method) {
        {
            auto host_values = headers.get_values("Host");
            if (host_values.size() != 1 || host_values.front().empty()) {
                return false;
            }

            std::string_view host_value = host_values.front();
            if (host_value.find(',') != std::string_view::npos || host_value.find(' ') != std::string_view::npos) {
                return false;
            }
        }

        {

            if (!validate_content_length_transfer_encoding(headers, body)) {
                return false;
            }

            auto content_length_value_opt = headers.get_value("Content-Length");
            if (method == EHttpRequestMethod::GET || method == EHttpRequestMethod::HEAD) {
                if (content_length_value_opt.has_value() && content_length_value_opt.value() != std::string("0")) {
                    return false;
                }
            }
        }
        return true;
    }

    bool validate_status(EHttpResponseStatus status) noexcept {
        return 
            status != EHttpResponseStatus::NOT_SET &&
            NEnum::enum_to_string(status).has_value();
    }

    bool validate_headers_response(const THttpHeaders& headers, std::string_view body, EHttpResponseStatus status) {
        {
            auto host_value_opt = headers.get_value("Host");
            if (host_value_opt.has_value()) {
                return false;
            }
        }

        {           
            auto date_values = headers.get_values("Date");
            if (date_values.size() > 1) {
                return false;
            } 
            if (date_values.size() == 1) {
                auto date = date_values.front();
                if (!NData::is_valid_http_date(date)) {
                    return false;
                }
            } 
        }

        {
            if (!validate_content_length_transfer_encoding(headers, body)) {
                return false;
            }
        }

        {
            auto content_length_value_opt = headers.get_value("Content-Length");
            if (status == EHttpResponseStatus::SWITCHING_PROTOCOLS || status == EHttpResponseStatus::NO_CONTENT) {
                if (content_length_value_opt.has_value()) {
                    return false;
                }
            } else if (status == EHttpResponseStatus::CREATED) {
                auto location_values = headers.get_values("Location");
                if (location_values.size() > 1) {
                    return false;
                } 
                if (location_values.size() == 1 && location_values.front().empty()) {
                    return false;
                }
            } else if (status == EHttpResponseStatus::UNAUTHORIZED) {
                auto authenticate_value_opt = headers.get_value("WWW-Authenticate");
                if (!authenticate_value_opt.has_value() || authenticate_value_opt.value().empty()) {
                    return false;
                }
            }
        }
        return true;
    }

} //namespace NHttp::NModel
