#include <library/http/error.h>
#include <library/http/response/response.h>
#include <library/http/model/validate.h>

namespace NHttp {

    void THttpResponse::set_version(THttpVersion version) {
        version_ = version;
    }

    void THttpResponse::set_status(EHttpResponseStatus status) {
        if (!NModel::validate_status(status)) {
            throw NError::THttpNotSetStatus();
        }
        status_ = status;
    }

    void THttpResponse::set_body(std::string body) {
        if (!NModel::validate_body(body)) {
            throw NError::THttpBadBody(body.size());
        }
        body_ = std::move(body);
    }

    const THttpVersion& THttpResponse::version() const noexcept {
        return version_;
    }

    EHttpResponseStatus THttpResponse::status() const noexcept {
        return status_;
    }
    
    const THttpHeaders& THttpResponse::headers() const noexcept {
        return headers_;
    }

    THttpHeaders& THttpResponse::headers() noexcept {
        return headers_;
    }

    const std::string& THttpResponse::body() const noexcept {
        return body_;
    }

    bool THttpResponse::valid() const {
        return NModel::validate_status(status_) && NModel::validate_headers_response(headers_, body_, status_);
    }

    std::ostream& operator<<(std::ostream& ostream, const THttpResponse& http_response) {
        http_response.serialize(ostream);
        return ostream;
    }

} // namespace NHttp
