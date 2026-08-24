#ifndef LIBRARY_HTTP_REQUEST_H
#error "Direct inclusion of this file is not allowed, include request.h"
// For the sake of sane code completion.
#include <library/http/request/request.h>
#endif

#include <library/common/enum/enum.h>

#include <library/http/model/validate.h>
#include <library/http/error.h>

#include <cctype>

namespace NHttp {

    template <typename TOstream>
    void THttpRequest::serialize(TOstream& ostream) const {

        if (!NModel::validate_version(version_)) {
            throw NError::THttpBadVersion(version_);
        }

        if (!NModel::validate_method(method_)) {
            throw NError::THttpNotSetMethod();
        }

        if (!NModel::validate_origin_form_target(target_)) {
            throw NError::THttpBadTarget(target_);
        }

        if (!NModel::validate_headers_request(headers_, body_, method_)) {
            throw NError::THttpConflictHeadersRequest();
        }

        ostream << NEnum::enum_to_string(method_).value() << " "
                << target_ << " "
                << "HTTP/" << version_.major << "." << version_.minor << "\r\n";

        
        for (const THttpHeader& header : headers_.items()) {
            ostream << header.name() << ": " << header.value() << "\r\n";
        }

        ostream << "\r\n";
        ostream << body_;
    }   

} // namespace NHttp
