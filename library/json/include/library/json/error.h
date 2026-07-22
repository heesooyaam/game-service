#pragma once

#include <library/json/interface.h>

#include <stdexcept>
#include <string>

namespace NJson::NError {

    class TJsonError : public std::runtime_error {
    public:
        TJsonError();

        explicit TJsonError(const std::string&);
        explicit TJsonError(const char*);
    };

    class TJsonTypeError : public TJsonError {
    public:
        TJsonTypeError();

        explicit TJsonTypeError(NJson::EJsonType expected, NJson::EJsonType actual);
        explicit TJsonTypeError(const std::string&);
        explicit TJsonTypeError(const char*);
    };

    class TJsonArrayOutOfRange : public TJsonError {
    public:
        TJsonArrayOutOfRange();

        explicit TJsonArrayOutOfRange(size_t, size_t);
    };

    class TJsonObjectOutOfRange : public TJsonError {
    public:
        TJsonObjectOutOfRange();
        
        explicit TJsonObjectOutOfRange(const std::string&);
    };

    class TJsonBadPath : public TJsonError {
    public:
        TJsonBadPath();

        explicit TJsonBadPath(const std::string&);
        explicit TJsonBadPath(const char*);
    };

    class TJsonBadDoubleNumber : public TJsonError {
    public:
        TJsonBadDoubleNumber();
    };

    class TJsonBadIntegerNumber : public TJsonError {
    public:
        TJsonBadIntegerNumber();
    };

} // namespace NJson::NError
