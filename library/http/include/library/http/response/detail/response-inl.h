#ifndef LIBRARY_HTTP_RESPONSE_H
#error "Direct inclusion of this file is not allowed, include response.h"
// For the sake of sane code completion.
#include <library/http/response/response.h>
#endif

#include <library/common/enum/enum.h>

#include <library/http/model/validate.h>
#include <library/http/model/date.h>
#include <library/http/error.h>

#include <cctype>

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

        for (const THttpHeader& header : headers_.items()) {
            ostream << header.name() << ": " << header.value() << "\r\n";
        }

        if (!headers_.get_value("Date").has_value() && static_cast<uint16_t>(status_) >= 200 && static_cast<uint16_t>(status_) < 500) {
            ostream << "Date: " << NData::get_current_http_date() << "\r\n";
        }
        
        ostream << "\r\n";
        ostream << body_;
    }   

} // namespace NHttp
