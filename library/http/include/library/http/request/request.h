#pragma once

#include <library/http/headers.h>
#include <library/http/version.h>
#include <library/http/request/methods.h>

#include <string>
#include <ostream>

namespace NHttp {

    class THttpRequest {
    public:

        void set_method(EHttpRequestMethod);
        void set_target(std::string);
        void set_version(THttpVersion);
        void set_body(std::string);

        void add_body(std::string);
        void reserve_body(size_t capacity);

        EHttpRequestMethod method() const noexcept;
        const std::string& target() const noexcept;
        const THttpVersion& version() const noexcept;

        const THttpHeaders& headers() const noexcept;
        THttpHeaders& headers() noexcept;

        const std::string& body() const noexcept;

        template <typename TOstream>
        void serialize(TOstream& ostream) const;

        bool valid() const;
        
    private:
        EHttpRequestMethod method_ = EHttpRequestMethod::NOT_SET;
        std::string target_;
        THttpVersion version_;
        THttpHeaders headers_;
        std::string body_;
    };

    std::ostream& operator<<(std::ostream&, const THttpRequest&);

} // namespace NHttp


#define LIBRARY_HTTP_REQUEST_H
#include <library/http/request/detail/request-inl.h>
#undef LIBRARY_HTTP_REQUEST_H
