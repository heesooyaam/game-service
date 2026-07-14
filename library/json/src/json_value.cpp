#include <json/json_value.h>

namespace {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class... Ts> overload(Ts...) -> overload<Ts...>;
}

namespace json::value {

    TJsonValue::TJsonValue(TNull null) 
        : root_value_(null)
    {}

    TJsonValue::TJsonValue(TNumber number)
        : root_value_(number)
    {}

    TJsonValue::TJsonValue(TBoolean boolean) 
        : root_value_(boolean)
    {}

    TJsonValue::TJsonValue(TString&& string) 
        : root_value_(std::move(string))
    {}

    TJsonValue::TJsonValue(TArray&& array) 
        : root_value_(std::make_unique<TArray>(std::move(array)))
    {}

    TJsonValue::TJsonValue(TObject&& object)
        : root_value_(std::make_unique<TObject>(std::move(object)))
    {}

    TJsonValue::TJsonValue(const TString& string)
        : root_value_(string)
    {}

    TJsonValue::TJsonValue(const TArray& array) 
        : root_value_(std::make_unique<TArray>(array))
    {}

    TJsonValue::TJsonValue(const TObject& object)
        : root_value_(std::make_unique<TObject>(object))
    {}

    TJsonValue::TJsonValue(const char* cstr)
        : root_value_(TString(cstr)) 
    {}
    
    TJsonValue& TJsonValue::operator=(TNull null) {
        root_value_ = null;
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TNumber number) {
        root_value_ = number;
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TBoolean boolean) {
        root_value_ = boolean;
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TString&& string) {
        root_value_ = std::move(string);
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TArray&& array) {
        root_value_ = std::make_unique<TArray>(std::move(array));
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TObject&& object) {
        root_value_ = std::make_unique<TObject>(std::move(object));
        return *this;
    }

    TJsonValue& TJsonValue::operator=(const TString& string) {
        root_value_ = string;
        return *this;
    }     

    TJsonValue& TJsonValue::operator=(const TArray& array) {
        root_value_ = std::make_unique<TArray>(array);
        return *this;
    }

    TJsonValue& TJsonValue::operator=(const TObject& object) {
        root_value_ = std::make_unique<TObject>(object);
        return *this;
    }

    TJsonValue::TJsonValue(const TJsonValue& other) 
        : root_value_(std::move(deep_copy(other).root_value_))
    {}

    TJsonValue::TJsonValue(TJsonValue&& other) noexcept 
        : root_value_(std::move(other.root_value_))
    {}

    TJsonValue& TJsonValue::operator=(const TJsonValue& other) {
        if (this == std::addressof(other)) {
            return *this;
        }
        
        root_value_ = std::move(deep_copy(other).root_value_);
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TJsonValue&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }

        root_value_ = std::move(other.root_value_);
        return *this;
    }

    TJsonValue TJsonValue::deep_copy(const TJsonValue& other) {
        return std::visit(overload{
            [](const std::monostate&) { return TJsonValue(); },
            [](const TNull& null) { return TJsonValue(null); },
            [](const TNumber& number) { return TJsonValue(number); },
            [](const TBoolean& boolean) { return TJsonValue(boolean); },
            [](const TString& string) { return TJsonValue(string); },
            [](const TArrayPtr& array_ptr) { 
                if (!array_ptr) {
                    return TJsonValue(TArray());
                }
                return TJsonValue(*array_ptr); 
            },
            [](const TObjectPtr& object_ptr) { 
                if (!object_ptr) {
                    return TJsonValue(TObject());
                }
                return TJsonValue(*object_ptr); 
            }
        }, other.root_value_);
    }

} //namespace json::value
