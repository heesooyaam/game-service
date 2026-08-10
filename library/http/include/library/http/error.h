#pragma once 

#include <library/http/version.h>

#include <stdexcept>
#include <string_view>

namespace NHttp::NError {

    class THttpError : public std::runtime_error {
    protected:
        explicit THttpError(std::string_view, std::string_view);
    };

    class THttpBadVersion : public THttpError {
    public:
        explicit THttpBadVersion(THttpVersion);
    };

    class THttpNotSetMethod : public THttpError {
    public:
        explicit THttpNotSetMethod();
    };

    class THttpNotSetStatus : public THttpError {
    public:
        explicit THttpNotSetStatus();
    };

    class THttpBadTarget : public THttpError {
    public:
        explicit THttpBadTarget(std::string_view);
    };

    class THttpBadHeaderName : public THttpError {
    public:
        explicit THttpBadHeaderName(std::string_view);
    };

    class THttpBadHeaderValue : public THttpError {
    public:
        explicit THttpBadHeaderValue(std::string_view);
    };

    class THttpConflictHeadersRequest : public THttpError {
    public:
        explicit THttpConflictHeadersRequest();
    };

    class THttpConflictHeadersResponse : public THttpError {
    public:
        explicit THttpConflictHeadersResponse();
    };

    class THttpBadBody : public THttpError {
    public:
        explicit THttpBadBody(size_t size);
    };

    class THttpTooManyHeaders : public THttpError {
    public:
        explicit THttpTooManyHeaders(size_t count);
    };

}  //namespace NHttp::NError
