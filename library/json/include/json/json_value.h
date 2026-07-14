#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <string>
#include <map>

namespace NJson {

    class TJsonValue;

    using TInteger = int32_t;
    using TDouble = double;
    
    using TNull = std::monostate;
    using TNumber = std::variant<TInteger, TDouble>;
    using TBoolean = bool;
    using TString = std::string;
    using TArray = std::vector<TJsonValue>;
    using TObject = std::map<TString, TJsonValue>;

    using TArrayPtr = std::unique_ptr<TArray>;
    using TObjectPtr = std::unique_ptr<TObject>;

    using TStorage = std::variant<TNull, TNumber, TBoolean, TString, TObjectPtr, TArrayPtr>;

    class TJsonValue {
    public:

        TJsonValue() = default;

        explicit TJsonValue(TNull);
        explicit TJsonValue(TInteger);
        explicit TJsonValue(TDouble);
        explicit TJsonValue(TBoolean);
        explicit TJsonValue(TString&&);
        explicit TJsonValue(TArray&&);
        explicit TJsonValue(TObject&&);
        explicit TJsonValue(const TString&);
        explicit TJsonValue(const TArray&);
        explicit TJsonValue(const TObject&);

        explicit TJsonValue(const char*);

        TJsonValue& operator=(TNull);
        TJsonValue& operator=(TInteger);
        TJsonValue& operator=(TDouble);
        TJsonValue& operator=(TBoolean);
        TJsonValue& operator=(TString&&);
        TJsonValue& operator=(TArray&&);
        TJsonValue& operator=(TObject&&);
        TJsonValue& operator=(const TString&);       
        TJsonValue& operator=(const TArray&);
        TJsonValue& operator=(const TObject&);

        TJsonValue& operator=(const char*);

        explicit TJsonValue(const TJsonValue&);
        explicit TJsonValue(TJsonValue&&) noexcept;

        TJsonValue& operator=(const TJsonValue&);
        TJsonValue& operator=(TJsonValue&&) noexcept;

        TStorage& get_root_value();
        const TStorage& get_root_value() const;

        bool operator==(const TJsonValue&) const;
        bool operator!=(const TJsonValue&) const;

        ~TJsonValue() = default;
    
    private:
        TStorage root_value_ = std::monostate();

        void clear();
        static TJsonValue deep_copy(const TJsonValue&);
        static bool deep_equal_check(const TJsonValue&, const TJsonValue&);
    };

} //namespace NJson 
