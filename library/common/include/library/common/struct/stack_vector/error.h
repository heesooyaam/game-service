#pragma once

#include <stdexcept>
#include <string_view>

namespace NCommon::NStruct::NError {

    class TCommonStructError : public std::runtime_error {
    protected:
        explicit TCommonStructError(std::string_view, std::string_view);
    };

    class TStackVectorError : public TCommonStructError {
    protected:
        explicit TStackVectorError(std::string_view, std::string_view);
    };

    class TStackVectorCapacityExceeded : public TStackVectorError {
    public:
        explicit TStackVectorCapacityExceeded(size_t capacity);
    };

} // namespace NCommon::NStruct::NError
