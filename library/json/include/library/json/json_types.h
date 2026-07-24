#pragma once 

#include <library/common/enum/enum.h>

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

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

    constexpr auto get_enum_entries(std::type_identity<EJsonType>) {
        return std::array{
            NEnum::TEnumEntry{EJsonType::Null, "NULL"},
            NEnum::TEnumEntry{EJsonType::Integer, "INTEGER"},
            NEnum::TEnumEntry{EJsonType::Double, "DOUBLE"},
            NEnum::TEnumEntry{EJsonType::Boolean, "BOOLEAN"},
            NEnum::TEnumEntry{EJsonType::String, "STRING"},
            NEnum::TEnumEntry{EJsonType::Array, "ARRAY"},
            NEnum::TEnumEntry{EJsonType::Object, "OBJECT"}
        };
    }

    class TJsonValue;

    using TNull = std::monostate;
    using TInteger = int64_t;
    using TDouble = double;
    using TBoolean = bool;
    using TString = std::string;
    using TArray = std::vector<TJsonValue>;
    using TObject = std::map<TString, TJsonValue>;

} // namespace NJson
