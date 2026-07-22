#pragma once 

#include <cstdint>

namespace NJson {

    enum class EJsonType : uint8_t {
        Null = 0,
        Integer,
        Double,
        Boolean,
        String,
        Array,
        Object
    };

    constexpr std::size_t JSON_TYPES_AMOUNT = 7;

} // namespace NJson
