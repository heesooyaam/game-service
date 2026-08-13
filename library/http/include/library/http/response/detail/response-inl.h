#ifndef LIBRARY_HTTP_RESPONSE_H
#error "Direct inclusion of this file is not allowed, include response.h"
// For the sake of sane code completion.
#include <library/http/response/response.h>
#endif

#include <library/common/enum/enum.h>

#include <library/http/model/validate.h>
#include <library/http/model/date.h>
#include <library/http/error.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <string_view>

namespace NHttp {

    template <typename TOstream>
    void THttpResponse::serialize(TOstream& ostream) const {

        if (!NModel::validate_version(version_)) {
            throw NError::THttpBadVersion(version_);
        }

        if (!NModel::validate_status(status_)) {
            throw NError::THttpNotSetStatus();
        }

        if (!NModel::validate_headers_response(headers_, body_, status_)) {
            throw NError::THttpConflictHeadersResponse();
        }

        ostream << "HTTP/" << version_.major << "." << version_.minor << " "
                << static_cast<uint16_t>(status_) << " "
                << NEnum::enum_to_string(status_).value() << "\r\n";

        auto comparator = [](std::string_view lhs, std::string_view rhs)  {
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                [](unsigned char a, unsigned char b) {
                    return std::tolower(a) < std::tolower(b);
                }
            );
        };
        std::set<std::string_view, decltype(comparator)> serialized_names(comparator);
        for (const THttpHeader& header : headers_.items()) {
            if (is_equal_case_insensitive(header.name(), "Set-Cookie")) {
                ostream << header.name() << ": " << header.value() << "\r\n";
                continue;
            } 

            if (!serialized_names.insert(header.name()).second) {
                continue;
            }

            char separator = ',';
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

        if (!serialized_names.contains("Date") && static_cast<uint16_t>(status_) >= 200 && static_cast<uint16_t>(status_) < 500) {
            ostream << "Date: " << NData::get_current_http_date() << "\r\n";
        }
        
        ostream << "\r\n";
        ostream << body_;
    }   

} //namespace NHttp
