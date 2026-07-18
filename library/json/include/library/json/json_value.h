#pragma once

#include <common/enum/enum.h>
#include <library/json/error.h>

#include <array>
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

    constexpr std::array<NEnum::TEnumEntry<EJsonType>, 7> get_enum_entries(std::type_identity<EJsonType>);

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

    template <typename T>
    concept CJsonNumber = CJsonInteger<T> || CJsonDouble<T>;

    class TJsonValue {
    public:

        TJsonValue() = default;

        TJsonValue(TNull) noexcept;

        template <CJsonInteger T>
        TJsonValue(T integer_number) noexcept;

        template <CJsonDouble T>
        TJsonValue(T double_number) noexcept;

        TJsonValue(TBoolean) noexcept;
        TJsonValue(TString&&);
        TJsonValue(TArray&&);
        TJsonValue(TObject&&);
        TJsonValue(const TString&);
        TJsonValue(const TArray&);
        TJsonValue(const TObject&);

        TJsonValue(const char*);

        TJsonValue& operator=(TNull) noexcept;

        template <CJsonInteger T>
        TJsonValue& operator=(T integer_number) noexcept;

        template <CJsonDouble T>
        TJsonValue& operator=(T double_number) noexcept;

        TJsonValue& operator=(TBoolean) noexcept;
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

        EJsonType get_value_type() const noexcept;

        bool is_null() const noexcept;
        bool is_integer() const noexcept;
        bool is_double() const noexcept;
        bool is_boolean() const noexcept;
        bool is_string() const noexcept;
        bool is_array() const noexcept;
        bool is_object() const noexcept;

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

        //for arrays
        size_t size() const;

        template <CJsonNumber T>
        TJsonValue& operator+=(T value);

        template <CJsonNumber T>
        TJsonValue& operator-=(T value);

        template <CJsonNumber T>
        TJsonValue& operator*=(T value);

        template <CJsonNumber T>
        TJsonValue& operator/=(T value);

        TJsonValue& operator+=(const TString&);

        TJsonValue& operator[](size_t);
        const TJsonValue& operator[](size_t) const;

        TJsonValue& at(size_t);
        const TJsonValue& at(size_t) const;
        
        TJsonValue& operator[](const TString&);
        TJsonValue& at(const TString&);
        const TJsonValue& at(const TString&) const;
        
        bool contains(const TString&) const;

        TJsonValue& get_and_create_value_by_path(std::string_view);

        TJsonValue& get_value_by_path(std::string_view);
        const TJsonValue& get_value_by_path(std::string_view) const;

    private:

        using TStorage = std::variant<TNull, TInteger, TDouble, TBoolean, TString, TArrayPtr, TObjectPtr>;

        TStorage root_value_ = std::monostate();

        void clear();
        static TJsonValue deep_copy(const TJsonValue&);
        static bool deep_equality_check(const TJsonValue&, const TJsonValue&);

        TArray& not_safe_get_array();
        TObject& not_safe_get_object();
        const TArray& not_safe_get_array() const;
        const TObject& not_safe_get_object() const;
    };

} //namespace NJson


#define LIBRARY_JSON_VALUE_H
#include <library/json/detail/json_value-inl.h>
#undef LIBRARY_JSON_VALUE_H
