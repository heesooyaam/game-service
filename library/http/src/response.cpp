#include <library/http/response/response.h>
#include <library/http/error.h>

namespace NHttp {

    void THttpResponse::set_version(THttpVersion version) {
        if (version != THttpVersion(1, 1)) {
            throw NError::THttpBadVersion(version);
        }
        version_ = version;
    }

    void THttpResponse::set_status(EHttpResponseStatus status) noexcept {
        status_ = status;
    }

    void THttpResponse::set_body(std::string body) {
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

} //namespace NHttp
