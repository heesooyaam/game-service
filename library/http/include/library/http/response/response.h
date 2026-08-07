#pragma once

#include <library/http/headers.h>
#include <library/http/version.h>
#include <library/http/response/statuses.h>

#include <string>

namespace NHttp {

    class THttpResponse {
    public:

        void set_version(THttpVersion) noexcept;
        void set_status(EHttpResponseStatus) noexcept;
        void set_body(std::string);

        const THttpVersion& version() const noexcept;
        EHttpResponseStatus status() const noexcept;

        const THttpHeaders& headers() const noexcept;
        THttpHeaders& headers() noexcept;

        const std::string& body() const noexcept;

    private:
        THttpVersion version_;
        EHttpResponseStatus status_;
        THttpHeaders headers_;
        std::string body_;
    };

} //namespace NHttp
