#include <common/enum/enum.h>
#include <common/overload.h>
#include <library/json/error.h>
#include <library/json/json_value.h>

#include <array>
#include <cassert>
#include <cctype>
#include <charconv>
#include <system_error>
#include <optional>
#include <format>

namespace {

    std::optional<size_t> parse_array_index(std::string_view data) noexcept {

        if (data.empty()) {
            return std::nullopt;
        }

        if (data.size() > 1 && data.front() == '0') {
            return std::nullopt;
        }

        size_t index = 0;

        const auto [ptr, error] = std::from_chars(
            data.data(),
            data.data() + data.size(),
            index
        );

        if (
            error != std::errc{} ||
            ptr != data.data() + data.size()
        ) {
            return std::nullopt;
        }

        return index;
    }

    void type_error(NJson::EJsonType expected, NJson::EJsonType actual) {

        auto opt_expected_type = NEnum::enum_to_string(expected);
        assert(opt_expected_type.has_value());

        auto opt_actual_type = NEnum::enum_to_string(actual);
        assert(opt_actual_type.has_value());

        throw NJson::NError::TJsonTypeError(
            std::format("[BAD ACCESS ERROR]: Expected {}, but got {}", opt_expected_type.value(), opt_actual_type.value())
        );
    }

    void index_error(size_t array_size, size_t index) {
        throw NJson::NError::TJsonBadArrayIndex(std::format("[BAD ARRAY INDEX]: array size = {}, index = {}", array_size, index));
    }

    void key_error(std::string_view key) {
        throw NJson::NError::TJsonBadObjectKey(std::format("[BAD OBJECT KEY]: {}", key));
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
        return static_cast<EJsonType>(root_value_.index());
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
            type_error(EJsonType::Integer, get_value_type());
        }
        return std::get<TInteger>(root_value_);
    }

    TDouble& TJsonValue::get_double() {
        if (!is_double()) {
            type_error(EJsonType::Double, get_value_type());
        }
        return std::get<TDouble>(root_value_);
    }

    TBoolean& TJsonValue::get_boolean() {
        if (!is_boolean()) {
            type_error(EJsonType::Boolean, get_value_type());   
        }
        return std::get<TBoolean>(root_value_);
    }

    TString& TJsonValue::get_string() {
        if (!is_string()) {
            type_error(EJsonType::String, get_value_type());        
        }
        return std::get<TString>(root_value_);
    }

    TArray& TJsonValue::get_array() {
        if (!is_array()) {
            type_error(EJsonType::Array, get_value_type());        
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    TObject& TJsonValue::get_object() {
        if (!is_object()) {
            type_error(EJsonType::Object, get_value_type());        
        }
        return *std::get<TObjectPtr>(root_value_);
    }

    const TInteger& TJsonValue::get_integer() const {
        if (!is_integer()) {
            type_error(EJsonType::Integer, get_value_type());        
        }
        return std::get<TInteger>(root_value_);
    }

    const TDouble& TJsonValue::get_double() const {
        if (!is_double()) {
            type_error(EJsonType::Double, get_value_type());        
        }
        return std::get<TDouble>(root_value_);
    }

    const TBoolean& TJsonValue::get_boolean() const {
        if (!is_boolean()) {
            type_error(EJsonType::Boolean, get_value_type());        
        }
        return std::get<TBoolean>(root_value_);
    }

    const TString& TJsonValue::get_string() const {
        if (!is_string()) {
            type_error(EJsonType::String, get_value_type());        
        }
        return std::get<TString>(root_value_);
    }

    const TArray& TJsonValue::get_array() const {
        if (!is_array()) {
            type_error(EJsonType::Array, get_value_type());        
        }
        return *std::get<TArrayPtr>(root_value_);
    }

    const TObject& TJsonValue::get_object() const {
        if (!is_object()) {
            type_error(EJsonType::Object, get_value_type());        
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

    TJsonValue& TJsonValue::get_and_create_value_by_path(std::string_view path) {
        TJsonValue* ptr = this;
        size_t start = 0;

        while (start <= path.size()) {

            size_t end = path.find('/', start);

            TString current;

            if (end == std::string_view::npos) {
                current = path.substr(start);
            } else {
                current = path.substr(start, end - start);
            }

            std::optional<size_t> opt_index = parse_array_index(current);

            if (opt_index.has_value()) {

                const size_t index = opt_index.value();
                
                if (!ptr->is_array()) {
                    type_error(EJsonType::Array, ptr->get_value_type());        
                }

                TArray& ref_array = ptr->not_safe_get_array();

                if (index >= ref_array.size()) {
                    index_error(ref_array.size(), index);
                }

                ptr = std::addressof(ref_array[index]);

            } else {

                if (ptr->is_null()) {
                    *ptr = TObject();
                }

                if (!ptr->is_object()) {
                    type_error(EJsonType::Object, ptr->get_value_type());        
                }
                
                TObject& ref_object = ptr->not_safe_get_object();
                ptr = std::addressof(ref_object[current]);
            }

            if (end == std::string_view::npos) {
                break;
            }

            start = end + 1;
        }

        return *ptr;
    }

    TJsonValue& TJsonValue::get_value_by_path(std::string_view path) {
        
        TJsonValue* ptr = this;
        size_t start = 0;

        while (start <= path.size()) {

            size_t end = path.find('/', start);

            TString current;

            if (end == std::string_view::npos) {
                current = path.substr(start);
            } else {
                current = path.substr(start, end - start);
            }

            std::optional<size_t> opt_index = parse_array_index(current);

            if (opt_index.has_value()) {

                const size_t index = opt_index.value();
                
                if (!ptr->is_array()) {
                    type_error(EJsonType::Array, ptr->get_value_type());        
                }

                TArray& ref_array = ptr->not_safe_get_array();

                if (index >= ref_array.size()) {
                    index_error(ref_array.size(), index);
                }

                ptr = std::addressof(ref_array[index]);

            } else {

                if (!ptr->is_object()) {
                    type_error(EJsonType::Object, ptr->get_value_type());        
                }
                
                TObject& ref_object = ptr->not_safe_get_object();
                auto iter = ref_object.find(current);

                if (iter == ref_object.end()) {
                    key_error(current);
                }

                ptr = std::addressof(iter->second);
            }

            if (end == std::string_view::npos) {
                break;
            }

            start = end + 1;
        }

        return *ptr;
    }
    
    const TJsonValue& TJsonValue::get_value_by_path(std::string_view path) const {

        const TJsonValue* ptr = this;

       size_t start = 0;

        while (start <= path.size()) {

            size_t end = path.find('/', start);

            TString current;

            if (end == std::string_view::npos) {
                current = path.substr(start);
            } else {
                current = path.substr(start, end - start);
            }

            std::optional<size_t> opt_index = parse_array_index(current);

            if (opt_index.has_value()) {

                const size_t index = opt_index.value();
                
                if (!ptr->is_array()) {
                    type_error(EJsonType::Array, ptr->get_value_type());        
                }

                const TArray& cref_array = ptr->not_safe_get_array();

                if (index >= cref_array.size()) {
                    index_error(cref_array.size(), index);
                }

                ptr = std::addressof(cref_array[index]);

            } else {

                if (!ptr->is_object()) {
                    type_error(EJsonType::Object, ptr->get_value_type());        
                }
                
                const TObject& cref_object = ptr->not_safe_get_object();
                auto iter = cref_object.find(current);

                if (iter == cref_object.end()) {
                    key_error(current);
                }
                ptr = std::addressof(iter->second);
            }

            if (end == std::string_view::npos) {
                break;
            }

            start = end + 1;
        }
        return *ptr;
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
                []<typename TLeft, typename TRight>(const TLeft& lhs, const TRight& rhs) {
                    return false;
                }
            }, 
            lhs.root_value_,
            rhs.root_value_
        );
    }


    TArray& TJsonValue::not_safe_get_array() {
        return *std::get<TArrayPtr>(root_value_);
    }

    TObject& TJsonValue::not_safe_get_object() {
        return *std::get<TObjectPtr>(root_value_);
    }

    const TArray& TJsonValue::not_safe_get_array() const {
        return *std::get<TArrayPtr>(root_value_);
    }

    const TObject& TJsonValue::not_safe_get_object() const {
        return *std::get<TObjectPtr>(root_value_);
    }

} //namespace NJson
