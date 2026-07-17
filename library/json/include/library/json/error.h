#pragma once

#include <stdexcept>
#include <string>

namespace NJson::NError {

    class TJsonTypeError : public std::runtime_error {
    public:
        TJsonTypeError()
            : std::runtime_error("[BAD ACCESS ERROR]")
        {}

        explicit TJsonTypeError(const std::string& msg) 
            : std::runtime_error(msg) 
        {}

        explicit TJsonTypeError(const char* msg) 
            : std::runtime_error(msg) 
        {} 
    };  

    class TJsonBadArrayIndex : public std::runtime_error {
    public:
        TJsonBadArrayIndex()
            : std::runtime_error("[BAD ARRAY INDEX]")
        {}

        explicit TJsonBadArrayIndex(const std::string& msg) 
            : std::runtime_error(msg) 
        {}

        explicit TJsonBadArrayIndex(const char* msg) 
            : std::runtime_error(msg) 
        {} 
    };  


    class TJsonBadObjectKey : public std::runtime_error {
    public:
        TJsonBadObjectKey()
            : std::runtime_error("[BAD OBJECT KEY]")
        {}

        explicit TJsonBadObjectKey(const std::string& msg) 
            : std::runtime_error(msg) 
        {}

        explicit TJsonBadObjectKey(const char* msg) 
            : std::runtime_error(msg) 
        {} 
    };  
        
} // namespace NJson::NError
