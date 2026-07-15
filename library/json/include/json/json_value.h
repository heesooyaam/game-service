#pragma once

#include "error.h"

#include <concepts>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
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

    class TJsonValue;

    using TNull = std::monostate;
    using TInteger = int64_t;
    using TDouble = double;
    using TBoolean = bool;
    using TString = std::string;
    using TArray = std::vector<TJsonValue>;
    using TObject = std::map<TString, TJsonValue>;

    using TArrayPtr = std::unique_ptr<TArray>;
    using TObjectPtr = std::unique_ptr<TObject>;

    using TStorage = std::variant<TNull, TInteger, TDouble, TBoolean, TString, TArrayPtr, TObjectPtr>;

    template <typename T>
    concept CCharacter =
        std::same_as<std::remove_cvref_t<T>, char> ||
        std::same_as<std::remove_cvref_t<T>, signed char> ||
        std::same_as<std::remove_cvref_t<T>, unsigned char> ||
        std::same_as<std::remove_cvref_t<T>, wchar_t> ||
        std::same_as<std::remove_cvref_t<T>, char8_t> ||
        std::same_as<std::remove_cvref_t<T>, char16_t> ||
        std::same_as<std::remove_cvref_t<T>, char32_t>;

    template <typename T>
    concept CJsonInteger =
        std::integral<std::remove_cvref_t<T>> &&
        !std::same_as<std::remove_cvref_t<T>, bool> &&
        !CCharacter<T>;

    template <typename T>
    concept CJsonDouble =
        std::floating_point<std::remove_cvref_t<T>>;

    class TJsonValue {
    public:

        TJsonValue() = default;

        TJsonValue(TNull);

        template <CJsonInteger T>
        TJsonValue(T integer_number)
            : root_value_(static_cast<TInteger>(integer_number))
        {}

        template <CJsonDouble T>
        TJsonValue(T double_number)
            : root_value_(static_cast<TDouble>(double_number))
        {}

        TJsonValue(TBoolean);
        TJsonValue(TString&&);
        TJsonValue(TArray&&);
        TJsonValue(TObject&&);
        TJsonValue(const TString&);
        TJsonValue(const TArray&);
        TJsonValue(const TObject&);

        TJsonValue(const char*);

        TJsonValue& operator=(TNull);

        template <CJsonInteger T>
        TJsonValue& operator=(T integer_number) {
            root_value_ = static_cast<TInteger>(integer_number);
            return *this;
        }

        template <CJsonDouble T>
        TJsonValue& operator=(T double_number) {
            root_value_ = static_cast<TDouble>(double_number);
            return *this;
        }

        TJsonValue& operator=(TBoolean);
        TJsonValue& operator=(TString&&);
        TJsonValue& operator=(TArray&&);
        TJsonValue& operator=(TObject&&);
        TJsonValue& operator=(const TString&);
        TJsonValue& operator=(const TArray&);
        TJsonValue& operator=(const TObject&);

        TJsonValue& operator=(const char*);

        TJsonValue(const TJsonValue&);
        TJsonValue(TJsonValue&&) noexcept;

        TJsonValue& operator=(const TJsonValue&);
        TJsonValue& operator=(TJsonValue&&) noexcept;

        EJsonType get_value_type() const;

        bool is_null() const;
        bool is_integer() const;
        bool is_double() const;
        bool is_boolean() const;
        bool is_string() const;
        bool is_array() const;
        bool is_object() const;

        bool operator==(const TJsonValue&) const;
        bool operator!=(const TJsonValue&) const;

        TInteger& get_integer();
        TDouble& get_double();
        TBoolean& get_boolean();
        TString& get_string();
        TArray& get_array();
        TObject& get_object();

        const TInteger& get_integer() const;
        const TDouble& get_double() const;
        const TBoolean& get_boolean() const;
        const TString& get_string() const;
        const TArray& get_array() const;
        const TObject& get_object() const;

        template <typename T>
        requires CJsonInteger<T> || CJsonDouble<T>
        TJsonValue& operator+=(T value) {
            if (is_integer()) {
                std::get<TInteger>(root_value_) += value;
                return *this;
            }

            if (is_double()) {
                std::get<TDouble>(root_value_) += value;
                return *this;
            }

            throw NError::TAccessError("Bad Access Error: Json is not a number");
        }

        template <typename T>
        requires CJsonInteger<T> || CJsonDouble<T>
        TJsonValue& operator-=(T value) {
            if (is_integer()) {
                std::get<TInteger>(root_value_) -= value;
                return *this;
            }

            if (is_double()) {
                std::get<TDouble>(root_value_) -= value;
                return *this;
            }

            throw NError::TAccessError("Bad Access Error: Json is not a number");
        }

        template <typename T>
        requires CJsonInteger<T> || CJsonDouble<T>
        TJsonValue& operator*=(T value) {
            if (is_integer()) {
                std::get<TInteger>(root_value_) *= value;
                return *this;
            }

            if (is_double()) {
                std::get<TDouble>(root_value_) *= value;
                return *this;
            }

            throw NError::TAccessError("Bad Access Error: Json is not a number");
        }

        template <typename T>
        requires CJsonInteger<T> || CJsonDouble<T>
        TJsonValue& operator/=(T value) {
            if (is_integer()) {
                std::get<TInteger>(root_value_) /= value;
                return *this;
            }

            if (is_double()) {
                std::get<TDouble>(root_value_) /= value;
                return *this;
            }

            throw NError::TAccessError("Bad Access Error: Json is not a number");
        }

        TJsonValue& operator+=(const TString&);

        TJsonValue& operator[](size_t);
        const TJsonValue& operator[](size_t) const;

        TJsonValue& at(size_t);
        const TJsonValue& at(size_t) const;
        
        TJsonValue& operator[](const TString&);
        TJsonValue& at(const TString&);
        const TJsonValue& at(const TString&) const;
        
        bool contains(const TString&) const;

        TJsonValue& get_value_by_path(std::string_view);
        const TJsonValue& get_value_by_path(std::string_view) const;

        ~TJsonValue() = default;

    private:
        TStorage root_value_ = std::monostate();

        void clear();
        static TJsonValue deep_copy(const TJsonValue&);
        static bool deep_equality_check(const TJsonValue&, const TJsonValue&);
    };

} //namespace NJson
