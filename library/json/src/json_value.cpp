#include <json/json_value.h>
#include <game/common/overload.h>

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

    TStorage& TJsonValue::get_root_value() {
        return root_value_;
    }

    const TStorage& TJsonValue::get_root_value() const {
        return root_value_;
    }

    bool TJsonValue::operator==(const TJsonValue& other) const {
        return deep_equality_check(*this, other);
    }
     
    bool TJsonValue::operator!=(const TJsonValue& other) const {
        return !(*this == other);
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
