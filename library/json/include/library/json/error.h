#pragma once

#include <stdexcept>
#include <string>

namespace NJson::NError {

    class TJsonTypeError : public std::runtime_error {
    public:
        TJsonTypeError();

        explicit TJsonTypeError(const std::string& msg);
        explicit TJsonTypeError(const char* msg);
    };  

    class TJsonBadArrayIndex : public std::runtime_error {
    public:
        TJsonBadArrayIndex();

        explicit TJsonBadArrayIndex(const std::string& msg);
        explicit TJsonBadArrayIndex(const char* msg);
    };  


    class TJsonBadObjectKey : public std::runtime_error {
    public:
        TJsonBadObjectKey();

        explicit TJsonBadObjectKey(const std::string& msg);
        explicit TJsonBadObjectKey(const char* msg);
    };  
        
} // namespace NJson::NError
