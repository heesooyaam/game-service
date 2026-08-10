#ifndef LIBRARY_HTTP_REQUEST_H
#error "Direct inclusion of this file is not allowed, include request.h"
// For the sake of sane code completion.
#include <library/http/request/request.h>
#endif

#include <library/common/enum/enum.h>

#include <library/http/model/validate.h>
#include <library/http/error.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <string_view>

namespace NHttp {

    template <typename TOstream>
    void THttpRequest::serialize(TOstream& ostream) const {
        if (!NModel::validate_method(method_)) {
            throw NError::THttpNotSetMethod();
        }

        if (!NModel::validate_target(target_)) {
            throw NError::THttpBadTarget(target_);
        }

        if (!NModel::validate_headers_request(headers_, body_, method_)) {
            throw NError::THttpConflictHeadersRequest();
        }

        ostream << NEnum::enum_to_string(method_).value() << " "
                << target_ << " "
                << "HTTP/" << version_.major << "." << version_.minor << "\r\n";

        struct TCaseInsensitiveCompare {
            bool operator()(std::string_view lhs, std::string_view rhs) const {
                return std::lexicographical_compare(
                    lhs.begin(), lhs.end(),
                    rhs.begin(), rhs.end(),
                    [](unsigned char a, unsigned char b) {
                        return std::tolower(a) < std::tolower(b);
                    }
                );
            }
        };

        std::set<std::string_view, TCaseInsensitiveCompare> serialized_names;
        for (const THttpHeader& header : headers_.items()) {
            if (!serialized_names.insert(header.name()).second) {
                continue;
            }

            char separator = ',';
            if (is_equal_case_insensitive(header.name(), "Cookie")) {
                separator = ';';
            }

            auto header_values = headers_.get_values(header.name());
            ostream << header.name() << ": ";
            for (size_t i = 0; i < header_values.size(); ++i) {
                ostream << header_values[i];
                if (i + 1 != header_values.size()) {
                    ostream << separator << " ";
                } 
            }
            ostream << "\r\n";
        }

        ostream << "\r\n";
        ostream << body_;
    }   

} //namespace NHttp
