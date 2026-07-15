#pragma once

#include <stdexcept>
#include <string>

namespace NJson::NError {

    class TAccessError : public std::runtime_error {
    public:
        TAccessError()
            : std::runtime_error("Bad Access Error")
        {}

        explicit TAccessError(const std::string& msg) 
            : std::runtime_error(msg) 
        {}

        explicit TAccessError(const char* msg) 
            : std::runtime_error(msg) 
        {} 
    };  

} // namespace NJson::NError
