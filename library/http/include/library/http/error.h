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

}  //namespace NHttp::NError
