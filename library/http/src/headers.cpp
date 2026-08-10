#include <library/http/headers.h>
#include <library/http/model/validate.h>
#include <library/http/error.h>

#include <algorithm>
#include <optional>
#include <string_view>

namespace NHttp {

    THttpHeader::THttpHeader(std::string name, std::string value)
        : name_(std::move(name))
        , value_(std::move(value))
    {}

    const std::string& THttpHeader::name() const noexcept {
        return name_;
    }

    const std::string& THttpHeader::value() const noexcept {
        return value_;
    }

    void THttpHeaders::add(std::string name, std::string value) {

        if (!NModel::validate_headers_count(headers_.size() + 1)) {
            throw NError::THttpTooManyHeaders(headers_.size());
        }

        if (!NModel::validate_header_name_general(name)) {
            throw NError::THttpBadHeaderName(name);
        }

        if (!NModel::validate_header_value_general(value)) {
            throw NError::THttpBadHeaderValue(value);
        }

        headers_.emplace_back(std::move(name), std::move(value));
    }

    std::optional<std::string_view> THttpHeaders::get_value(std::string_view name) const noexcept {
        auto it = std::find_if(headers_.begin(), headers_.end(), [name](const THttpHeader& element) {
            return is_equal_case_insensitive(element.name(), name);
        });   
        if (it == headers_.end()) {
            return std::nullopt;
        }
        return it->value();
    }

    std::vector<std::string_view> THttpHeaders::get_values(std::string_view name) const {
        std::vector<std::string_view> values;
        std::for_each(headers_.begin(), headers_.end(), [name, &values](const THttpHeader& element) {
            if (is_equal_case_insensitive(element.name(), name)) {
                values.push_back(element.value());
            }
        });
        return values;
    }

    const std::vector<THttpHeader>& THttpHeaders::items() const noexcept {
        return headers_;
    }

    bool is_equal_case_insensitive(std::string_view lhs, std::string_view rhs) noexcept {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(lhs[i])) != 
                std::tolower(static_cast<unsigned char>(rhs[i]))) {
                return false;
            }
        }
        return true;
    }

} //namespace NHttp
