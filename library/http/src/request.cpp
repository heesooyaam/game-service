#include <library/http/error.h>
#include <library/http/request/request.h>
#include <library/http/model/validate.h>

namespace NHttp {

    void THttpRequest::set_method(EHttpRequestMethod method) {
        if (!NModel::validate_method(method)) {
            throw NError::THttpNotSetMethod();
        }
        method_ = method;
    }

    void THttpRequest::set_target(std::string target) {
        if (!NModel::validate_origin_form_target(target)) {
            throw NError::THttpBadTarget(target);
        }
        target_ = std::move(target);
    }

    void THttpRequest::set_version(THttpVersion version) {
        version_ = version;
    }

    void THttpRequest::set_body(std::string body) {
        if (!NModel::validate_body(body)) {
            throw NError::THttpBadBody(body.size());
        }
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

    bool THttpRequest::valid() const {
        return NModel::validate_method(method_) && NModel::validate_origin_form_target(target_) && NModel::validate_headers_request(headers_, body_, method_);
    }

    std::ostream& operator<<(std::ostream& ostream, const THttpRequest& http_request) {
        http_request.serialize(ostream);
        return ostream;
    }

} //namespace NHttp
