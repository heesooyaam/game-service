#include <game/common/overload.h>
#include <json/error.h>
#include <json/json_value.h>

#include <cassert>

namespace NJson {

    TJsonValue::TJsonValue(TNull null) 
        : root_value_(null)
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

    TJsonValue& TJsonValue::operator=(const char* cstr) {
        root_value_ = TString(cstr);
        return *this;
    }

    TJsonValue::TJsonValue(const TJsonValue& other) 
        : root_value_(std::move(deep_copy(other).root_value_))
    {}

    TJsonValue::TJsonValue(TJsonValue&& other) noexcept 
        : root_value_(std::move(other.root_value_))
    {
        other.clear();
    }

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
        other.clear();
        return *this;
    }

    EJsonType TJsonValue::get_value_type() const {
        return static_cast<EJsonType>(root_value_.index());
    }

    bool TJsonValue::is_null() const {
        return std::holds_alternative<TNull>(root_value_);
    }

    bool TJsonValue::is_integer() const {
        return std::holds_alternative<TInteger>(root_value_);
    }

    bool TJsonValue::is_double() const {
        return std::holds_alternative<TDouble>(root_value_);
    }

    bool TJsonValue::is_boolean() const {
        return std::holds_alternative<TBoolean>(root_value_);
    }

    bool TJsonValue::is_string() const {
        return std::holds_alternative<TString>(root_value_);
    }

    bool TJsonValue::is_array() const {
        return std::holds_alternative<TArrayPtr>(root_value_);
    }

    bool TJsonValue::is_object() const {
        return std::holds_alternative<TObjectPtr>(root_value_);
    }

    bool TJsonValue::operator==(const TJsonValue& other) const {
        return deep_equality_check(*this, other);
    }
     
    bool TJsonValue::operator!=(const TJsonValue& other) const {
        return !(*this == other);
    }

    TInteger& TJsonValue::get_integer() {
        if (!is_integer()) {
            throw NError::TAccessError("Bad Access Error: Json is not integer");
        }
        return std::get<TInteger>(root_value_);
    }

    TDouble& TJsonValue::get_double() {
        if (!is_double()) {
            throw NError::TAccessError("Bad Access Error: Json is not double");
        }
        return std::get<TDouble>(root_value_);
    }

    TBoolean& TJsonValue::get_boolean() {
        if (!is_boolean()) {
            throw NError::TAccessError("Bad Access Error: Json is not boolean");
        }
        return std::get<TBoolean>(root_value_);
    }

    TString& TJsonValue::get_string() {
        if (!is_string()) {
            throw NError::TAccessError("Bad Access Error: Json is not string");
        }
        return std::get<TString>(root_value_);
    }

    TArray& TJsonValue::get_array() {
        if (!is_array()) {
            throw NError::TAccessError("Bad Access Error: Json is not array");
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    TObject& TJsonValue::get_object() {
        if (!is_object()) {
            throw NError::TAccessError("Bad Access Error: Json is not  object");
        }
        return *std::get<TObjectPtr>(root_value_);
    }

    const TInteger& TJsonValue::get_integer() const {
        if (!is_integer()) {
            throw NError::TAccessError("Bad Access Error: Json is not integer");
        }
        return std::get<TInteger>(root_value_);
    }

    const TDouble& TJsonValue::get_double() const {
        if (!is_double()) {
            throw NError::TAccessError("Bad Access Error: Json is not double");
        }
        return std::get<TDouble>(root_value_);
    }

    const TBoolean& TJsonValue::get_boolean() const {
        if (!is_boolean()) {
            throw NError::TAccessError("Bad Access Error: Json is not boolean");
        }
        return std::get<TBoolean>(root_value_);
    }

    const TString& TJsonValue::get_string() const {
        if (!is_string()) {
            throw NError::TAccessError("Bad Access Error: Json is not string");
        }
        return std::get<TString>(root_value_);
    }

    const TArray& TJsonValue::get_array() const {
        if (!is_array()) {
            throw NError::TAccessError("Bad Access Error: Json is not array");
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    const TObject& TJsonValue::get_object() const {
        if (!is_object()) {
            throw NError::TAccessError("Bad Access Error: Json is not  object");
        }
        return *std::get<TObjectPtr>(root_value_);
    }

    TJsonValue& TJsonValue::operator+=(const TString& value) {
        get_string() += value;
        return *this;
    }

    TJsonValue& TJsonValue::operator[](size_t index) {
        return get_array()[index];
    }
    
    const TJsonValue& TJsonValue::operator[](size_t index) const {
        return get_array()[index];
    }


    TJsonValue& TJsonValue::at(size_t index) {
        return get_array().at(index);
    }
    
    const TJsonValue& TJsonValue::at(size_t index) const {
        return get_array().at(index);    
    }

    TJsonValue& TJsonValue::operator[](const TString& key) {
        return get_object()[key];
    }
        
    TJsonValue& TJsonValue::at(const TString& key) {
        return get_object().at(key);
    }

    const TJsonValue& TJsonValue::at(const TString& key) const {
        return get_object().at(key);
    }

    bool TJsonValue::contains(const TString& key) const {
        return get_object().contains(key);
    }

    void TJsonValue::clear() {
        root_value_ = TNull();
    }

    TJsonValue TJsonValue::deep_copy(const TJsonValue& other) {
        return std::visit(
            NCommon::TOverload{
                [](const TNull&) { 
                    return TJsonValue(); 
                },
                [](const TInteger& integer_number) { 
                    return TJsonValue(integer_number);
                },
                [](const TDouble& double_number) {
                    return TJsonValue(double_number);
                },
                [](const TBoolean& boolean) {
                    return TJsonValue(boolean); 
                },
                [](const TString& string) { 
                    return TJsonValue(string); 
                },
                [](const TArrayPtr& array_ptr) { 
                    assert(array_ptr);
                    return TJsonValue(*array_ptr); 
                },
                [](const TObjectPtr& object_ptr) { 
                    assert(object_ptr);
                    return TJsonValue(*object_ptr); 
                }
            }, 
            other.root_value_
        );
    }

    bool TJsonValue::deep_equality_check(const TJsonValue& lhs, const TJsonValue& rhs) {
        return std::visit(
            NCommon::TOverload{
                [](const TArrayPtr& lhs_ptr, const TArrayPtr& rhs_ptr) {
                    assert(lhs_ptr);
                    assert(rhs_ptr);
                    return *lhs_ptr == *rhs_ptr;
                },
                [](const TObjectPtr& lhs_ptr, const TObjectPtr& rhs_ptr) {
                    assert(lhs_ptr);
                    assert(rhs_ptr);
                    return *lhs_ptr == *rhs_ptr;
                },
                []<typename T>(const T& lhs, const T& rhs) {
                    return lhs == rhs;
                },
                []<typename TLeft, typename TRight>(const TLeft& lhs, const TRight& rhs) {
                    return false;
                }
            }, 
            lhs.root_value_,
            rhs.root_value_
        );
    }

} //namespace NJson
