#ifndef LIBRARY_JSON_VALUE_H
#error "Direct inclusion of this file is not allowed, include json_value.h"
// For the sake of sane code completion.
#include <library/json/json_value.h>
#endif

#include <library/json/error.h>

#include <cmath>
#include <utility>

namespace NJson::NPrivate {

    template <CJsonInteger T>
    TInteger checked_integer(T value) {
        if (!std::in_range<TInteger>(value)) {
            throw NError::TJsonIntegerOutOfRange(value);
        }
        return static_cast<TInteger>(value);
    }

    template <CJsonFloatingPoint T>
    TDouble checked_floating_point(T value) {
        const long double extended_value =
            static_cast<long double>(value);

        static constexpr long double max_value =
            static_cast<long double>(
                std::numeric_limits<TDouble>::max()
            );

        if (
            !std::isfinite(extended_value) ||
            extended_value < -max_value ||
            extended_value > max_value
        ) {
            throw NError::TJsonBadDoubleNumber(value);
        }

        return static_cast<TDouble>(value);
    }

}

namespace NJson {

    template <CJsonInteger T>
    TJsonValue::TJsonValue(T integer_number)
        : root_value_(NPrivate::checked_integer(integer_number))
    {}

    template <CJsonFloatingPoint T>
    TJsonValue::TJsonValue(T floating_point_number)
        : root_value_(NPrivate::checked_floating_point(floating_point_number))
    {}

    template <CJsonInteger T>
    TJsonValue& TJsonValue::operator=(T integer_number) {
        root_value_ = NPrivate::checked_integer(integer_number);
        return *this;
    }

    template <CJsonFloatingPoint T>
    TJsonValue& TJsonValue::operator=(T floating_point_number) {
        root_value_ = NPrivate::checked_floating_point(floating_point_number);
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
        throw NError::TJsonOperationError(get_value_type());
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
        throw NError::TJsonOperationError(get_value_type());
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

        throw NError::TJsonOperationError(get_value_type());
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

        throw NError::TJsonOperationError(get_value_type());
    }

} //namespace NJson
