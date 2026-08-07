#include <library/http/request/request.h>

namespace NHttp {

    void THttpRequest::set_method(EHttpRequestMethod method) noexcept {
        method_ = method;
    }

    void THttpRequest::set_target(std::string target) {
        target_ = std::move(target);
    }

    void THttpRequest::set_version(THttpVersion version) noexcept {
        version_ = version;
    }

    void THttpRequest::set_body(std::string body) {
        body_ = std::move(body);
    }

    EHttpRequestMethod THttpRequest::method() const noexcept {
        return method_;
    }

    const std::string& THttpRequest::target() const noexcept {
        return target_;
    }

    const THttpVersion& THttpRequest::version() const noexcept {
        return version_;
    }

    const THttpHeaders& THttpRequest::headers() const noexcept {
        return headers_;
    }

    THttpHeaders& THttpRequest::headers() noexcept {
        return headers_;
    }

    const std::string& THttpRequest::body() const noexcept {
        return body_;
    }

} //namespace NHttp
