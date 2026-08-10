#pragma once

#include <library/http/headers.h>
#include <library/http/version.h>
#include <library/http/response/statuses.h>

#include <string>
#include <ostream>

namespace NHttp {

    class THttpResponse {
    public:

        void set_version(THttpVersion);
        void set_status(EHttpResponseStatus);
        void set_body(std::string);

        const THttpVersion& version() const noexcept;
        EHttpResponseStatus status() const noexcept;

        const THttpHeaders& headers() const noexcept;
        THttpHeaders& headers() noexcept;

        const std::string& body() const noexcept;

        template <typename TOstream>
        void serialize(TOstream& ostream) const;

        bool valid() const;

    private:
        THttpVersion version_;
        EHttpResponseStatus status_ = EHttpResponseStatus::NOT_SET;
        THttpHeaders headers_;
        std::string body_;
    };

    std::ostream& operator<<(std::ostream&, const THttpResponse&);

} //namespace NHttp


#define LIBRARY_HTTP_RESPONSE_H
#include <library/http/response/detail/response-inl.h>
#undef LIBRARY_JSON_VALUE_
