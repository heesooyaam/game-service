#include <library/json/error.h>

namespace NJson::NError {

    TJsonTypeError::TJsonTypeError()
        : std::runtime_error("[BAD ACCESS ERROR]")
    {}

    TJsonTypeError::TJsonTypeError(const std::string& msg) 
        : std::runtime_error(msg) 
    {}

    TJsonTypeError::TJsonTypeError(const char* msg) 
        : std::runtime_error(msg) 
    {} 

    TJsonBadArrayIndex::TJsonBadArrayIndex()
        : std::runtime_error("[BAD ARRAY INDEX]")
    {}

    TJsonBadArrayIndex::TJsonBadArrayIndex(const std::string& msg) 
        : std::runtime_error(msg) 
    {}

    TJsonBadArrayIndex::TJsonBadArrayIndex(const char* msg) 
        : std::runtime_error(msg) 
    {} 

    TJsonBadObjectKey::TJsonBadObjectKey()
        : std::runtime_error("[BAD OBJECT KEY]")
    {}

    TJsonBadObjectKey::TJsonBadObjectKey(const std::string& msg) 
        : std::runtime_error(msg) 
    {}

    TJsonBadObjectKey::TJsonBadObjectKey(const char* msg) 
        : std::runtime_error(msg) 
    {} 
        
} // namespace NJson::NError
