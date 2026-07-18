#ifndef LIBRARY_JSON_VALUE_H
#error "Direct inclusion of this file is not allowed, include asr.h"
// For the sake of sane code completion.
#include <library/json/json.h>
#endif

namespace NJson {

    constexpr std::array<NEnum::TEnumEntry<EJsonType>, 7> get_enum_entries(std::type_identity<EJsonType>) {
        return {{
            {EJsonType::Null, "NULL"},
            {EJsonType::Integer, "INTEGER"},
            {EJsonType::Double, "DOUBLE"},
            {EJsonType::Boolean, "BOOLEAN"},
            {EJsonType::String, "STRING"},
            {EJsonType::Array, "ARRAY"},
            {EJsonType::Object, "OBJECT"}
        }};
    }

    template <CJsonInteger T>
    TJsonValue::TJsonValue(T integer_number) noexcept
        : root_value_(static_cast<TInteger>(integer_number))
    {}

    template <CJsonDouble T>
    TJsonValue::TJsonValue(T double_number) noexcept
        : root_value_(static_cast<TDouble>(double_number))
    {}


    template <CJsonInteger T>
    TJsonValue& TJsonValue::operator=(T integer_number) noexcept {
        root_value_ = static_cast<TInteger>(integer_number);
        return *this;
    }

    template <CJsonDouble T>
    TJsonValue& TJsonValue::operator=(T double_number) noexcept {
        root_value_ = static_cast<TDouble>(double_number);
        return *this;
    }

    template <CJsonNumber T>
    TJsonValue& TJsonValue::operator+=(T value)  {
        if (is_integer()) {
            std::get<TInteger>(root_value_) += value;
            return *this;
        }

        if (is_double()) {
            std::get<TDouble>(root_value_) += value;
            return *this;
        }

        throw NError::TJsonTypeError("[BAD ACCESS ERROR]: Json is not a number");
    }

    template <CJsonNumber T>
    TJsonValue& TJsonValue::operator-=(T value) {
        if (is_integer()) {
            std::get<TInteger>(root_value_) -= value;
            return *this;
        }

        if (is_double()) {
            std::get<TDouble>(root_value_) -= value;
            return *this;
        }

        throw NError::TJsonTypeError("[BAD ACCESS ERROR]: Json is not a number");
    }

    template <CJsonNumber T>
    TJsonValue& TJsonValue::operator*=(T value) {
        if (is_integer()) {
            std::get<TInteger>(root_value_) *= value;
            return *this;
        }

        if (is_double()) {
            std::get<TDouble>(root_value_) *= value;
            return *this;
        }

        throw NError::TJsonTypeError("[BAD ACCESS ERROR]: Json is not a number");
    }

    template <CJsonNumber T>
    TJsonValue& TJsonValue::operator/=(T value) {
        if (is_integer()) {
            std::get<TInteger>(root_value_) /= value;
            return *this;
        }

        if (is_double()) {
            std::get<TDouble>(root_value_) /= value;
            return *this;
        }

        throw NError::TJsonTypeError("[BAD ACCESS ERROR]: Json is not a number");
    }

} //namespace NJson
