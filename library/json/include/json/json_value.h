#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <string>
#include <unordered_map>

namespace json::value {

    class TJsonValue;

    struct TNull{
        bool operator==(const TNull& other) const { 
            return true;
        }
    };
    
    using TNumber = double;
    using TBoolean = bool;
    using TString = std::string;
    using TArray = std::vector<TJsonValue>;
    using TObject = std::unordered_map<TString, TJsonValue>;

    using TArrayPtr = std::unique_ptr<TArray>;
    using TObjectPtr = std::unique_ptr<TObject>;

    using TStorage = std::variant<TNull, TNumber, TBoolean, TString, TObjectPtr, TArrayPtr, std::monostate>;

    class TJsonValue {
    public:

        TJsonValue() = default;

        explicit TJsonValue(TNull);
        explicit TJsonValue(TNumber);
        explicit TJsonValue(TBoolean);
        explicit TJsonValue(TString&&);
        explicit TJsonValue(TArray&&);
        explicit TJsonValue(TObject&&);
        explicit TJsonValue(const TString&);
        explicit TJsonValue(const TArray&);
        explicit TJsonValue(const TObject&);

        explicit TJsonValue(const char*);

        TJsonValue& operator=(TNull);
        TJsonValue& operator=(TNumber);
        TJsonValue& operator=(TBoolean);
        TJsonValue& operator=(TString&&);
        TJsonValue& operator=(TArray&&);
        TJsonValue& operator=(TObject&&);
        TJsonValue& operator=(const TString&);       
        TJsonValue& operator=(const TArray&);
        TJsonValue& operator=(const TObject&);

        explicit TJsonValue(const TJsonValue&);
        explicit TJsonValue(TJsonValue&&) noexcept;

        TJsonValue& operator=(const TJsonValue&);
        TJsonValue& operator=(TJsonValue&&) noexcept;

        TStorage& get_root_value();
        const TStorage& get_root_value() const;

        bool operator==(const TJsonValue& other) const;
        bool operator!=(const TJsonValue& other) const;

        ~TJsonValue() = default;
    
    private:
        TStorage root_value_ = std::monostate();

        static TJsonValue deep_copy(const TJsonValue&);
        static bool deep_equal_check(const TJsonValue&, const TJsonValue&);
    };

} //namespace json::value  
