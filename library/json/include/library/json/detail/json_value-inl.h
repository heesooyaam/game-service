#ifndef LIBRARY_JSON_VALUE_H
#error "Direct inclusion of this file is not allowed, include json_value.h"
// For the sake of sane code completion.
#include <library/json/json_value.h>
#endif

#include <library/json/error.h>

#include <cmath>
#include <utility>

namespace {

    template <NJson::CJsonInteger T>
    NJson::TInteger checked_integer(T value) {
        if (!std::in_range<NJson::TInteger>(value)) {
            throw NJson::NError::TJsonBadIntegerNumber();
        }
        return static_cast<NJson::TInteger>(value);
    }

    template <NJson::CJsonFloatingPoint T>
    NJson::TDouble checked_floating_point(T value) {
        if (!std::isfinite(value) || std::isnan(value)) {
            throw NJson::NError::TJsonBadDoubleNumber();
        }
        return static_cast<NJson::TDouble>(value);
    }

}

namespace NJson {

    template <CJsonInteger T>
    TJsonValue::TJsonValue(T integer_number)
        : root_value_(checked_integer(integer_number))
    {}

    template <CJsonFloatingPoint T>
    TJsonValue::TJsonValue(T floating_point_number)
        : root_value_(checked_floating_point(floating_point_number))
    {}

    template <CJsonInteger T>
    TJsonValue& TJsonValue::operator=(T integer_number) {
        root_value_ = checked_integer(integer_number);
        return *this;
    }

    template <CJsonFloatingPoint T>
    TJsonValue& TJsonValue::operator=(T floating_point_number) {
        root_value_ = checked_floating_point(floating_point_number);
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
        throw NError::TJsonTypeError("json is not a number");
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
        throw NError::TJsonTypeError("json is not a number");
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

        throw NError::TJsonTypeError("json is not a number");
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

        throw NError::TJsonTypeError("json is not a number");
    }

} //namespace NJson
