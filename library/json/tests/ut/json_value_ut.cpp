#include <library/json/error.h>
#include <library/json/json_value.h>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

constexpr void static_check(bool value) {
    if (!value) {
        std::exit(EXIT_FAILURE);
    }
}

void check(bool value) {
    if (!value) {
        std::exit(EXIT_FAILURE);
    }
}

namespace NJson::NTests {

    void test_concepts() {
        // 1. Тесты для CCharacter
        static_check(CCharacter<char>);
        static_check(CCharacter<wchar_t>);
        static_check(CCharacter<char32_t>);
        static_check(CCharacter<const char&>); 
        static_check(CCharacter<unsigned char&&>); 
        static_check(!CCharacter<int>);        
        static_check(!CCharacter<double>);     

        // 2. Тесты для CJsonInteger
        static_check(CJsonInteger<int>);
        static_check(CJsonInteger<long long>);
        static_check(CJsonInteger<std::uint64_t>);
        static_check(CJsonInteger<short>);
        static_check(CJsonInteger<const int&>); 
        static_check(!CJsonInteger<bool>);      
        static_check(!CJsonInteger<char>);      
        static_check(!CJsonInteger<double>);    

        // 3. Тесты для CJsonFloatingPoint
        static_check(CJsonFloatingPoint<float>);
        static_check(CJsonFloatingPoint<double>);
        static_check(CJsonFloatingPoint<long double>);
        static_check(CJsonFloatingPoint<const double&>); 
        static_check(!CJsonFloatingPoint<int>);          
        static_check(!CJsonFloatingPoint<bool>);         

        // 4. Тесты для CJsonNumber
        static_check(CJsonNumber<int>);
        static_check(CJsonNumber<double>);
        static_check(CJsonNumber<float>);
        static_check(CJsonNumber<size_t>);
        static_check(CJsonNumber<const volatile long&&>);
        static_check(!CJsonNumber<bool>);
        static_check(!CJsonNumber<char>);
        static_check(!CJsonNumber<std::nullptr_t>);
        static_check(!CJsonNumber<void*>);
    }

    void test_constructors() {
        // Null
        TJsonValue null_val;
        TJsonValue null_val2(TNull{});
        check(null_val.is_null());
        check(null_val2.is_null());
        check(null_val.get_value_type() == EJsonType::Null);

        // Integer
        TJsonValue int_val = 42;
        check(int_val.is_integer());
        check(int_val.get_integer() == 42);
        check(int_val.get_value_type() == EJsonType::Integer);

        // Double
        TJsonValue double_val = 3.14;
        check(double_val.is_double());
        check(double_val.get_double() == 3.14);

        // Boolean
        TJsonValue bool_val(true);
        check(bool_val.is_boolean());
        check(bool_val.get_boolean() == true);

        // String
        TJsonValue str_val1("c_string");
        TJsonValue str_val2(TString("std_string"));
        check(str_val1.is_string() && str_val1.get_string() == "c_string");
        check(str_val2.is_string() && str_val2.get_string() == "std_string");

        // Array
        TArray arr;
        arr.emplace_back(1);
        arr.emplace_back(2);
        TJsonValue arr_val(std::move(arr));
        check(arr_val.is_array());
        check(arr_val.size() == 2);
        check(arr_val.get_array()[0].get_integer() == 1);

        // Object
        TObject obj;
        obj["key"] = 100;
        TJsonValue obj_val(std::move(obj));
        check(obj_val.is_object());
        check(obj_val.get_object().at("key").get_integer() == 100);
    }


    void test_assignments() {
        TJsonValue val;
        
        val = 100;
        check(val.is_integer() && val.get_integer() == 100);
        
        val = 3.1415;
        check(val.is_double() && val.get_double() == 3.1415);

        val = true;
        check(val.is_boolean() && val.get_boolean() == true);

        val = "reassigned";
        check(val.is_string() && val.get_string() == "reassigned");
        
        val = TArray{TJsonValue(1), TJsonValue(2)};
        check(val.is_array() && val.size() == 2);

        val = TObject{{"test", TJsonValue("value")}};
        check(val.is_object() && val.get_object()["test"].get_string() == "value");

        val = TNull{};
        check(val.is_null());
    }

    void test_copy_semantics() {
        TJsonValue original = TArray{TJsonValue(1), TJsonValue(2)};
        
        // Copy constructor
        TJsonValue copied_construct(original);
        check(copied_construct.is_array());
        check(copied_construct == original);

        // Copy assignment
        TJsonValue copied_assign;
        copied_assign = original;
        check(copied_assign == original);

        // Deep copy verification
        copied_assign.get_array()[0] = 99;
        check(copied_assign != original);
        check(original.get_array()[0].get_integer() == 1);

        // Self-assignment copy
        copied_assign = copied_assign;
        check(copied_assign.is_array() && copied_assign.get_array()[0].get_integer() == 99);
    }

    void test_move_semantics() {
        TJsonValue original = TString("movable_string");
        
        // Move constructor
        TJsonValue moved_construct(std::move(original));
        check(moved_construct.is_string() && moved_construct.get_string() == "movable_string");
        check(original.is_null());

        // Move assignment
        TJsonValue original_obj = TObject{{"key", 42}};
        TJsonValue moved_assign;
        moved_assign = std::move(original_obj);
        
        check(moved_assign.is_object() && moved_assign.get_object()["key"].get_integer() == 42);
        check(original_obj.is_null());

        // Self-assignment move
        moved_assign = std::move(moved_assign);
        check(moved_assign.is_object());
    }

    void test_equality() {
        check(TJsonValue(42) == TJsonValue(42));
        check(TJsonValue(42) != TJsonValue(43));
        check(TJsonValue(42) != TJsonValue(42.0)); // Strict type checking

        check(TJsonValue("test") == TJsonValue("test"));
        check(TJsonValue("test") != TJsonValue("test2"));

        TJsonValue arr1 = TArray{1, 2, 3};
        TJsonValue arr2 = TArray{1, 2, 3};
        TJsonValue arr3 = TArray{1, 2, 4};
        check(arr1 == arr2);
        check(arr1 != arr3);

        TJsonValue obj1 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj2 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj3 = TObject{{"a", 1}, {"b", false}};
        check(obj1 == obj2);
        check(obj1 != obj3);
    }

    void test_deep_nesting() {
        TObject inner_obj;
        inner_obj["key"] = "value";

        TArray inner_arr;
        inner_arr.emplace_back(std::move(inner_obj));
        inner_arr.emplace_back(42);

        TObject root_obj;
        root_obj["data"] = std::move(inner_arr);

        TJsonValue root_json(std::move(root_obj));
        TJsonValue root_copy(root_json);
        
        check(root_copy == root_json);

        root_copy.get_object()["data"].get_array()[0].get_object()["key"] = "NEW_VALUE";

        check(root_copy != root_json);
        check(root_json.get_object()["data"].get_array()[0].get_object()["key"].get_string() == "value");
    }

    void test_getters_and_exceptions() {
        TJsonValue i = 42;
        TJsonValue s = "string";

        check(i.get_integer() == 42);
        check(s.get_string() == "string");

        bool thrown = false;
        try { i.get_string(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);

        thrown = false;
        try { s.get_array(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);

        thrown = false;
        try { i.get_object(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);
    }

    void test_const_correctness() {
        const TJsonValue const_val = TArray{1, 2, 3};
        
        check(const_val.is_array());
        check(const_val.size() == 3);
        check(const_val.get_array()[0].get_integer() == 1);
        check(const_val.get_array()[1].get_integer() == 2);
    }

    void test_unique_ownership_copy() {
        TJsonValue original_arr = TArray{TJsonValue(1), TJsonValue("test")};
        TJsonValue copied_arr = original_arr;

        const auto* orig_ptr = &original_arr.get_array()[0];
        const auto* copy_ptr = &copied_arr.get_array()[0];
        check(orig_ptr != copy_ptr);

        const std::string& orig_str = original_arr.get_array()[1].get_string();
        const std::string& copy_str = copied_arr.get_array()[1].get_string();
        check(&orig_str != &copy_str);
    }

    void test_unique_ownership_move() {
        TJsonValue original_arr = TArray{TJsonValue(10), TJsonValue(20)};
        const auto* memory_address_before = &original_arr.get_array()[0];

        TJsonValue moved_arr = std::move(original_arr);
        const auto* memory_address_after = &moved_arr.get_array()[0];
        
        check(memory_address_before == memory_address_after);
        check(original_arr.is_null());
    }

    void test_implicit_numeric_conversions() {
        short s = 10;
        unsigned int ui = 20;
        size_t st = 30;
        long long ll = 40;

        TJsonValue val1 = s;
        TJsonValue val2 = ui;
        TJsonValue val3 = st;
        TJsonValue val4 = ll;

        check(val1.get_integer() == 10);
        check(val2.get_integer() == 20);
        check(val3.get_integer() == 30);
        check(val4.get_integer() == 40);

        float f = 3.14f;
        TJsonValue val5 = f;
        check(val5.is_double());
    }

    void test_math_operations() {
        TJsonValue i = 10;
        i += 5;   check(i.get_integer() == 15);
        i -= 2;   check(i.get_integer() == 13);
        i *= 2;   check(i.get_integer() == 26);
        i /= 2;   check(i.get_integer() == 13);

        TJsonValue d = 2.5;
        d += 1.5; check(d.get_double() == 4.0);
        d -= 1.0; check(d.get_double() == 3.0);
        d *= 3.0; check(d.get_double() == 9.0);
        d /= 2.0; check(d.get_double() == 4.5);

        TJsonValue s = "hello";
        s += TString(" world");
        check(s.get_string() == "hello world");

        bool thrown = false;
        try { s += 5; } catch (const NError::TJsonOperationError&) { thrown = true; }
        check(thrown);
    }

    void test_array_access() {
        TJsonValue arr = TArray{TJsonValue(10), TJsonValue(20), TJsonValue(30)};

        check(arr[0].get_integer() == 10);
        arr[1] = 99;
        check(arr.at(1).get_integer() == 99);

        const TJsonValue const_arr = arr;
        check(const_arr[2].get_integer() == 30);
        check(const_arr.at(2).get_integer() == 30);

        bool thrown = false;
        try { arr.at(100); } catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        check(thrown);

        TJsonValue num = 42;
        thrown = false;
        try { num[0]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);
    }

    void test_object_access() {
        TJsonValue obj = TObject{{"name", TJsonValue("Alice")}, {"age", TJsonValue(25)}};

        check(obj.contains("name"));
        check(!obj.contains("height"));

        check(obj["name"].get_string() == "Alice");
        check(obj.at("age").get_integer() == 25);

        obj["height"] = 170.5;
        check(obj.at("height").get_double() == 170.5);

        const TJsonValue const_obj = obj;
        check(const_obj.at("name").get_string() == "Alice");
        
        bool thrown = false;
        try { const_obj.at("non_existent_key"); } catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        check(thrown);

        TJsonValue str = "string";
        thrown = false;
        try { str["key"]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);
    }

    void test_path_access() {
        TObject root;
        TArray users;
        
        TObject user1; 
        user1["id"] = 1; 
        user1["name"] = "Alice";
        
        TObject user2; 
        user2["id"] = 2; 
        user2["name"] = "Bob";
        
        users.emplace_back(std::move(user1));
        users.emplace_back(std::move(user2));
        
        root["users"] = std::move(users);
        root["metadata"] = TObject{{"count", 2}};
        
        TJsonValue json(std::move(root));

        // ПУСТОЙ ПУТЬ
        check(json.get_value_by_path("") == json);
        const TJsonValue& const_json = json;
        check(const_json.get_value_by_path("") == json);

        check(std::addressof(json.get_and_create_value_by_path("")) == std::addressof(json));

        // Корректный доступ по пути
        check(json.get_value_by_path("/users/0/id").get_integer() == 1);
        check(json.get_value_by_path("/users/1/name").get_string() == "Bob");
        check(json.get_value_by_path("/metadata/count").get_integer() == 2);

        // Проверка константного доступа
        check(const_json.get_value_by_path("/users/0/name").get_string() == "Alice");

        // Объект со строковым ключом, который выглядит как число
        TJsonValue dict = TObject{{"0", "zero_key_value"}};
        check(dict.get_value_by_path("/0").get_string() == "zero_key_value");

        // Исключения
        bool thrown = false;
        try { json.get_value_by_path("users/0"); } 
        catch (const NError::TJsonBadPath&) { thrown = true; }
        check(thrown);

        thrown = false;
        try { json.get_value_by_path("/users/5"); } 
        catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        check(thrown);

        thrown = false;
        try { json.get_value_by_path("/metadata/version"); } 
        catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        check(thrown);

        thrown = false;
        try { json.get_value_by_path("/metadata/count/value"); } 
        catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);
    }

    void test_get_and_create_value_by_path() {
        TJsonValue json = TNull{};

        // ПУСТОЙ ПУТЬ
        check(json.get_and_create_value_by_path("") == json);
        check(std::addressof(json.get_and_create_value_by_path("")) == std::addressof(json));

        // Базовое создание иерархии
        json.get_and_create_value_by_path("/config/server/port") = 8080;
        check(json.is_object());
        check(json.get_value_by_path("/config/server/port").get_integer() == 8080);

        // Добавление в уже существующий объект
        json.get_and_create_value_by_path("/config/server/host") = "localhost";
        check(json.get_value_by_path("/config/server/host").get_string() == "localhost");

        // Автоматическое создание
        json.get_and_create_value_by_path("/items/0") = "first";
        check(json.get_value_by_path("/items").is_object()); 
        check(json.get_value_by_path("/items/0").get_string() == "first");

        // Мутация существующего массива по индексу
        json.get_and_create_value_by_path("/real_array") = TArray{TJsonValue(10), TJsonValue(20)};
        json.get_and_create_value_by_path("/real_array/1") = 99;
        check(json.get_value_by_path("/real_array/1").get_integer() == 99);

        // Исключение
        bool thrown = false;
        try { json.get_and_create_value_by_path("/config/server/port/value"); } 
        catch (const NError::TJsonTypeError&) { thrown = true; }
        check(thrown);
    }

    void test_exception_text() {
        bool thrown = false;
        try {
            TJsonValue val = 42;
            val.get_string();
        } catch (const NError::TJsonTypeError& e) {
            thrown = true;
            std::string msg = e.what();
            check(msg.find("STRING") != std::string::npos);
            check(msg.find("INTEGER") != std::string::npos);
        }
        check(thrown);

        thrown = false;
        try {
            TJsonValue val = TArray{1, 2};
            val.at(5);
        } catch (const NError::TJsonArrayOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            check(msg.find("array size = 2") != std::string::npos);
            check(msg.find("index = 5") != std::string::npos);
        }
        check(thrown);

        thrown = false;
        try {
            TJsonValue val = TObject{{"key", 1}};
            val.at("missing_key");
        } catch (const NError::TJsonObjectOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            check(msg.find("[JSON OBJECT OUT OF RANGE]: missing_key") != std::string::npos);
        }
        check(thrown);
    }

    void test_numeric_edge_cases() {
        bool thrown = false;

        // 1. Тест переполнения целого числа
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<std::uint64_t>::max();
        } catch (const NError::TJsonIntegerOutOfRange&) {
            thrown = true;
        }
        check(thrown); 

        TJsonValue valid_int = std::numeric_limits<int64_t>::max();
        check(valid_int.get_integer() == std::numeric_limits<int64_t>::max());

        // 2. NaN
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<double>::quiet_NaN();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        check(thrown); 

        // 3. Infinity 
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<double>::infinity();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        check(thrown);

        // 4. -Infinity
        thrown = false;
        try {
            TJsonValue val = -std::numeric_limits<double>::infinity();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        check(thrown);

        TJsonValue valid_double = std::numeric_limits<double>::max();
        check(valid_double.get_double() == std::numeric_limits<double>::max());

        // 5. Тест оператора присваивания
        TJsonValue assign_val;
        
        thrown = false;
        try { assign_val = std::numeric_limits<std::uint64_t>::max(); } 
        catch (const NError::TJsonIntegerOutOfRange&) { thrown = true; }
        check(thrown);
        
        thrown = false;
        try { assign_val = std::numeric_limits<double>::infinity(); } 
        catch (const NError::TJsonBadDoubleNumber&) { thrown = true; }
        check(thrown);
    }

} // namespace NJson::NTests

int main() { 
    using namespace NJson::NTests;

    test_concepts();

    test_constructors();
    test_assignments();
    test_copy_semantics();
    test_move_semantics();
    test_equality();
    test_deep_nesting();
    test_getters_and_exceptions();
    test_const_correctness();

    test_unique_ownership_copy();
    test_unique_ownership_move();
    test_implicit_numeric_conversions();

    test_math_operations();
    test_array_access();
    test_object_access();
    
    test_path_access();
    test_get_and_create_value_by_path();

    test_exception_text();
    test_numeric_edge_cases(); 

    std::cout << "All TJsonValue tests passed successfully! You are breathtaking!" << std::endl;
    return 0;
}
