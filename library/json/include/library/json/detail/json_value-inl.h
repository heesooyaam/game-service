#ifndef LIBRARY_JSON_VALUE_H
#error "Direct inclusion of this file is not allowed, include json_value.h"
// For the sake of sane code completion.
#include <library/json/json_value.h>
#endif

#include <library/json/error.h>

#include <charconv>
#include <cmath>
#include <utility>

namespace {

    std::optional<size_t> parse_array_index(std::string_view data) noexcept {

        if (data.empty()) {
            return std::nullopt;
        }

        if (data.size() > 1 && data.front() == '0') {
            return std::nullopt;
        }
        
        size_t index = 0;
        const auto [ptr, error] = std::from_chars(
            data.data(),
            data.data() + data.size(),
            index
        );

        if (
            error != std::errc{} ||
            ptr != data.data() + data.size()
        ) {
            return std::nullopt;
        }

        return index;
    }

}   

namespace NJson {

    constexpr std::array<NEnum::TEnumEntry<EJsonType>, JSON_TYPES_AMOUNT> get_enum_entries(std::type_identity<EJsonType>) {
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
    TJsonValue::TJsonValue(T integer_number)
        : root_value_(static_cast<TInteger>(integer_number))
    {
        if (!std::in_range<TInteger>(integer_number)) {
            throw NError::TJsonBadIntegerNumber();
        }
    }

    template <CJsonDouble T>
    TJsonValue::TJsonValue(T double_number)
        : root_value_(static_cast<TDouble>(double_number))
    {
        if (!std::isfinite(double_number) || std::isnan(double_number)) {
            throw NError::TJsonBadDoubleNumber();
        }
    }

    template <CJsonInteger T>
    TJsonValue& TJsonValue::operator=(T integer_number) {
        if (!std::in_range<TInteger>(integer_number)) {
            throw NError::TJsonBadIntegerNumber();
        }

        root_value_ = static_cast<TInteger>(integer_number);
        return *this;
    }

    template <CJsonDouble T>
    TJsonValue& TJsonValue::operator=(T double_number) {
        if (!std::isfinite(double_number) || std::isnan(double_number)) {
            throw NError::TJsonBadDoubleNumber();
        }

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
        throw NError::TJsonTypeError("[JSON TYPE ERROR]: Json is not a number");
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
        throw NError::TJsonTypeError("[JSON TYPE ERROR]: Json is not a number");
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

        throw NError::TJsonTypeError("[JSON TYPE ERROR]: Json is not a number");
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

        throw NError::TJsonTypeError("[JSON TYPE ERROR]: Json is not a number");
    }

} //namespace NJson
