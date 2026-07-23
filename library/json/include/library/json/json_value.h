#pragma once

#include <library/json/detail/json_value_concepts.h>
#include <library/json/json_types.h>

#include <memory>
#include <string_view>
#include <variant>

namespace NJson {

    class TJsonValue {
    public:

        TJsonValue() = default;

        TJsonValue(TNull) noexcept;

        template <CJsonInteger T>
        TJsonValue(T integer_number);

        template <CJsonFloatingPoint T>
        TJsonValue(T floating_point_number);

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
        TJsonValue& operator=(T integer_number);

        template <CJsonFloatingPoint T>
        TJsonValue& operator=(T floating_point_number);

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
        using TArrayPtr = std::unique_ptr<TArray>;
        using TObjectPtr = std::unique_ptr<TObject>;
        using TStorage = std::variant<TNull, TInteger, TDouble, TBoolean, TString, TArrayPtr, TObjectPtr>;

        TStorage root_value_ = std::monostate();

        void clear();
        static TJsonValue deep_copy(const TJsonValue&);
        static bool deep_equality_check(const TJsonValue&, const TJsonValue&);        
    };

} //namespace NJson

#define LIBRARY_JSON_VALUE_H
#include <library/json/detail/json_value-inl.h>
#undef LIBRARY_JSON_VALUE_H
