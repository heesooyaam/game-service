#pragma once

#include <library/http/headers.h>
#include <library/http/version.h>
#include <library/http/request/methods.h>

#include <string>

namespace NHttp {

    class THttpRequest {
    public:

        void set_method(EHttpRequestMethod) noexcept;
        void set_target(std::string);
        void set_version(THttpVersion) noexcept;
        void set_body(std::string);

        EHttpRequestMethod method() const noexcept;
        const std::string& target() const noexcept;
        const THttpVersion& version() const noexcept;

        const THttpHeaders& headers() const noexcept;
        THttpHeaders& headers() noexcept;

        const std::string& body() const noexcept;

    private:
        EHttpRequestMethod method_;
        std::string target_;
        THttpVersion version_;
        THttpHeaders headers_;
        std::string body_;
    };

} //namespace NHttp
