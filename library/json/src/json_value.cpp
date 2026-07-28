#include <library/json/error.h>
#include <library/json/json_value.h>

#include <library/common/enum/enum.h>
#include <library/common/overloaded.h>
#include <library/common/parse_number.h>

#include <cassert>
#include <cctype>

namespace {

    std::optional<size_t> parse_array_index(std::string_view data) noexcept {
        if (data.empty()) {
            return std::nullopt;
        }

        if (data.size() > 1 && data.front() == '0') {
            return std::nullopt;
        }
        
        return NCommon::parse_number<size_t>(data); 
    }

    template <bool Create, typename TypeJsonValuePtr>
    auto get_value_by_path_impl(std::string_view path, TypeJsonValuePtr ptr_this) -> decltype(*std::declval<TypeJsonValuePtr>())& {
        using TypeArrayRef = decltype(std::declval<TypeJsonValuePtr>()->get_array());
        using TypeObjectRef = decltype(std::declval<TypeJsonValuePtr>()->get_object());

        if (path.empty()) {
            return *ptr_this;
        }

        if (path[0] != '/') {
            throw NJson::NError::TJsonBadPath(path, "first '/' skipped");
        }
        
        TypeJsonValuePtr ptr = ptr_this;
        size_t start = 1;

        while (start <= path.size()) {
            size_t end = path.find('/', start);
            NJson::TString current;
            if (end == std::string_view::npos) {
                current = path.substr(start);
            } else {
                current = path.substr(start, end - start);
            }

            std::optional<size_t> opt_index = parse_array_index(current);
            if (ptr->is_array()) {
                if (!opt_index.has_value()) {
                    throw NJson::NError::TJsonBadPath(path, "Invalid Index");
                }

                const size_t index = opt_index.value();
                TypeArrayRef reference_array = ptr->get_array();

                if (index >= reference_array.size()) {
                    throw NJson::NError::TJsonArrayOutOfRange(reference_array.size(), index);
                }

                ptr = std::addressof(reference_array[index]);
            } else {
                if constexpr (Create) {
                    if (ptr->is_null()) {
                        *ptr = NJson::TObject();
                    }

                    if (!ptr->is_object()) {
                        throw NJson::NError::TJsonTypeError(NJson::EJsonType::Object, ptr->get_value_type());
                    }
                    
                    TypeObjectRef reference_object = ptr->get_object();
                    ptr = std::addressof(reference_object[current]);
                } else {
                    if (!ptr->is_object()) {
                        throw NJson::NError::TJsonTypeError(NJson::EJsonType::Object, ptr->get_value_type());
                    }

                    TypeObjectRef reference_object = ptr->get_object();
                    auto iter = reference_object.find(current);

                    if (iter == reference_object.end()) {
                        throw NJson::NError::TJsonObjectOutOfRange(current);
                    }

                    ptr = std::addressof(iter->second);
                }
            }
            if (end == std::string_view::npos) {
                break;
            }
            start = end + 1;
        }
        return *ptr;
    }

}

namespace NJson {

    TJsonValue::TJsonValue(TNull null) noexcept
        : root_value_(null)
    {}

    TJsonValue::TJsonValue(TBoolean boolean) noexcept
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
    
    TJsonValue& TJsonValue::operator=(TNull null) noexcept {
        root_value_ = null;
        return *this;
    }

    TJsonValue& TJsonValue::operator=(TBoolean boolean) noexcept {
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

    EJsonType TJsonValue::get_value_type() const noexcept {
        return std::visit(
            NCommon::TOverloaded{
                [](const TNull&) { 
                    return EJsonType::Null; 
                },
                [](const TInteger&) { 
                    return EJsonType::Integer;
                },
                [](const TDouble&) {
                    return EJsonType::Double;
                },
                [](const TBoolean&) {
                    return EJsonType::Boolean;
                },
                [](const TString&) { 
                    return EJsonType::String;
                },
                [](const TArrayPtr& array_ptr) { 
                    assert(array_ptr);
                    return EJsonType::Array; 
                },
                [](const TObjectPtr& object_ptr) { 
                    assert(object_ptr);
                    return EJsonType::Object;
                }
            }, 
            root_value_
        );
    }

    bool TJsonValue::is_null() const noexcept {
        return std::holds_alternative<TNull>(root_value_);
    }

    bool TJsonValue::is_integer() const noexcept {
        return std::holds_alternative<TInteger>(root_value_);
    }

    bool TJsonValue::is_double() const noexcept {
        return std::holds_alternative<TDouble>(root_value_);
    }

    bool TJsonValue::is_boolean() const noexcept {
        return std::holds_alternative<TBoolean>(root_value_);
    }

    bool TJsonValue::is_string() const noexcept {
        return std::holds_alternative<TString>(root_value_);
    }

    bool TJsonValue::is_array() const noexcept {
        return std::holds_alternative<TArrayPtr>(root_value_);
    }

    bool TJsonValue::is_object() const noexcept {
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
            throw NError::TJsonTypeError(EJsonType::Integer, get_value_type());
        }
        return std::get<TInteger>(root_value_);
    }

    TDouble& TJsonValue::get_double() {
        if (!is_double()) {
            throw NError::TJsonTypeError(EJsonType::Double, get_value_type());
        }
        return std::get<TDouble>(root_value_);
    }

    TBoolean& TJsonValue::get_boolean() {
        if (!is_boolean()) {
            throw NError::TJsonTypeError(EJsonType::Boolean, get_value_type());   
        }
        return std::get<TBoolean>(root_value_);
    }

    TString& TJsonValue::get_string() {
        if (!is_string()) {
            throw NError::TJsonTypeError(EJsonType::String, get_value_type());        
        }
        return std::get<TString>(root_value_);
    }

    TArray& TJsonValue::get_array() {
        if (!is_array()) {
            throw NError::TJsonTypeError(EJsonType::Array, get_value_type());        
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    TObject& TJsonValue::get_object() {
        if (!is_object()) {
            throw NError::TJsonTypeError(EJsonType::Object, get_value_type());        
        }
        return *std::get<TObjectPtr>(root_value_);
    }

    const TInteger& TJsonValue::get_integer() const {
        if (!is_integer()) {
            throw NError::TJsonTypeError(EJsonType::Integer, get_value_type());        
        }
        return std::get<TInteger>(root_value_);
    }

    const TDouble& TJsonValue::get_double() const {
        if (!is_double()) {
            throw NError::TJsonTypeError(EJsonType::Double, get_value_type());        
        }
        return std::get<TDouble>(root_value_);
    }

    const TBoolean& TJsonValue::get_boolean() const {
        if (!is_boolean()) {
            throw NError::TJsonTypeError(EJsonType::Boolean, get_value_type());        
        }
        return std::get<TBoolean>(root_value_);
    }

    const TString& TJsonValue::get_string() const {
        if (!is_string()) {
            throw NError::TJsonTypeError(EJsonType::String, get_value_type());        
        }
        return std::get<TString>(root_value_);
    }

    const TArray& TJsonValue::get_array() const {
        if (!is_array()) {
            throw NError::TJsonTypeError(EJsonType::Array, get_value_type());        
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    const TObject& TJsonValue::get_object() const {
        if (!is_object()) {
            throw NError::TJsonTypeError(EJsonType::Object, get_value_type());        
        }
        return *std::get<TObjectPtr>(root_value_);
    }

    size_t TJsonValue::size() const {
        return get_array().size();
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
        TArray& reference_array = get_array();
        if (reference_array.size() <= index) {
            throw NError::TJsonArrayOutOfRange(reference_array.size(), index);
        }
        return reference_array[index];
    }
    
    const TJsonValue& TJsonValue::at(size_t index) const {
        const TArray& reference_array = get_array();
        if (reference_array.size() <= index) {
            throw NError::TJsonArrayOutOfRange(reference_array.size(), index);
        }
        return reference_array[index];   
    }

    TJsonValue& TJsonValue::operator[](const TString& key) {
        return get_object()[key];
    }
        
    TJsonValue& TJsonValue::at(const TString& key) {
        TObject& reference_object = get_object();
        auto it = reference_object.find(key);
        if (it == reference_object.end()) {
            throw NError::TJsonObjectOutOfRange(key);
        }
        return it->second;   
    }

    const TJsonValue& TJsonValue::at(const TString& key) const {
        const TObject& reference_object = get_object();
        auto it = reference_object.find(key);
        if (it == reference_object.end()) {
            throw NError::TJsonObjectOutOfRange(key);
        }
        return it->second;  
    }

    bool TJsonValue::contains(const TString& key) const {
        return get_object().contains(key);
    }

    TJsonValue& TJsonValue::get_and_create_value_by_path(std::string_view path) {
        return get_value_by_path_impl<true, decltype(this)>(path, this);
    }

    TJsonValue& TJsonValue::get_value_by_path(std::string_view path) {
        return get_value_by_path_impl<false, decltype(this)>(path, this);
    }
    
    const TJsonValue& TJsonValue::get_value_by_path(std::string_view path) const {
        return get_value_by_path_impl<false, decltype(this)>(path, this);
    }

    void TJsonValue::clear() {
        root_value_ = TNull();
    }

    TJsonValue TJsonValue::deep_copy(const TJsonValue& other) {
        return std::visit(
            NCommon::TOverloaded{
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
            NCommon::TOverloaded{
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
                []<typename TLeft, typename TRight>(const TLeft&, const TRight&) {
                    return false;
                }
            }, 
            lhs.root_value_,
            rhs.root_value_
        );
    }

} //namespace NJson
