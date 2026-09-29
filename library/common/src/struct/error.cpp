#include <library/common/struct/stack_vector/error.h>

#include <format>
#include <string_view>

namespace NCommon::NStruct::NError {

    TCommonStructError::TCommonStructError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    TStackVectorError::TStackVectorError(std::string_view label, std::string_view msg)
        : TCommonStructError(label, msg)
    {}

    TStackVectorCapacityExceeded::TStackVectorCapacityExceeded(size_t capacity)
        : TStackVectorError(
            "STACK VECTOR ERROR",
            std::format(
                "Capacity exceeded ( {} )",
                capacity
            )
        )
    {}

} // namespace NCommon::NStruct::NError

