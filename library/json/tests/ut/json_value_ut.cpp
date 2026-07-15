#include <json/json_value.h>
#include <json/error.h> 

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace NJson::NTests {

    void test_constructors() {
        // Null
        TJsonValue null_val;
        TJsonValue null_val2(TNull{});
        assert(null_val.is_null());
        assert(null_val2.is_null());
        assert(null_val.get_value_type() == EJsonType::Null);

        // Integer
        TJsonValue int_val = 42;
        assert(int_val.is_integer());
        assert(int_val.get_integer() == 42);
        assert(int_val.get_value_type() == EJsonType::Integer);

        // Double
        TJsonValue double_val = 3.14;
        assert(double_val.is_double());
        assert(double_val.get_double() == 3.14);

        // Boolean
        TJsonValue bool_val(true);
        assert(bool_val.is_boolean());
        assert(bool_val.get_boolean() == true);

        // String
        TJsonValue str_val1("c_string");
        TJsonValue str_val2(TString("std_string"));
        assert(str_val1.is_string() && str_val1.get_string() == "c_string");
        assert(str_val2.is_string() && str_val2.get_string() == "std_string");

        // Array
        TArray arr;
        arr.emplace_back(1);
        arr.emplace_back(2);
        TJsonValue arr_val(arr);
        assert(arr_val.is_array());
        assert(arr_val.get_array().size() == 2);
        assert(arr_val.get_array()[0].get_integer() == 1);

        // Object
        TObject obj;
        obj["key"] = 100;
        TJsonValue obj_val(obj);
        assert(obj_val.is_object());
        assert(obj_val.get_object().at("key").get_integer() == 100);
    }

    void test_assignments() {
        TJsonValue val;
        
        val = 100;
        assert(val.is_integer() && val.get_integer() == 100);
        
        val = 3.1415;
        assert(val.is_double() && val.get_double() == 3.1415);

        val = true;
        assert(val.is_boolean() && val.get_boolean() == true);

        val = "reassigned";
        assert(val.is_string() && val.get_string() == "reassigned");
        
        val = TArray{TJsonValue(1), TJsonValue(2)};
        assert(val.is_array() && val.get_array().size() == 2);

        val = TObject{{"test", TJsonValue("value")}};
        assert(val.is_object() && val.get_object()["test"].get_string() == "value");

        val = TNull{};
        assert(val.is_null());
    }

    void test_copy_semantics() {
        TJsonValue original = TArray{TJsonValue(1), TJsonValue(2)};
        
        // Copy constructor
        TJsonValue copied_construct(original);
        assert(copied_construct.is_array());
        assert(copied_construct == original);

        // Copy assignment
        TJsonValue copied_assign;
        copied_assign = original;
        assert(copied_assign == original);

        // Deep copy verification (changing copy shouldn't affect original)
        copied_assign.get_array()[0] = 99;
        assert(copied_assign != original);
        assert(original.get_array()[0].get_integer() == 1); // Original unchanged

        // Self-assignment copy
        copied_assign = copied_assign;
        assert(copied_assign.is_array() && copied_assign.get_array()[0].get_integer() == 99);
    }

    void test_move_semantics() {
        TJsonValue original = TString("movable_string");
        
        // Move constructor
        TJsonValue moved_construct(std::move(original));
        assert(moved_construct.is_string() && moved_construct.get_string() == "movable_string");
        assert(original.is_null()); // original should be cleared

        // Move assignment
        TJsonValue original_obj = TObject{{"key", 42}};
        TJsonValue moved_assign;
        moved_assign = std::move(original_obj);
        
        assert(moved_assign.is_object() && moved_assign.get_object()["key"].get_integer() == 42);
        assert(original_obj.is_null()); // original should be cleared

        // Self-assignment move
        moved_assign = std::move(moved_assign);
        assert(moved_assign.is_object());
    }

    void test_equality() {
        assert(TJsonValue(42) == TJsonValue(42));
        assert(TJsonValue(42) != TJsonValue(43));
        assert(TJsonValue(42) != TJsonValue(42.0)); // Strict type checking (1 != 1.0)

        assert(TJsonValue("test") == TJsonValue("test"));
        assert(TJsonValue("test") != TJsonValue("test2"));

        TJsonValue arr1 = TArray{1, 2, 3};
        TJsonValue arr2 = TArray{1, 2, 3};
        TJsonValue arr3 = TArray{1, 2, 4};
        assert(arr1 == arr2);
        assert(arr1 != arr3);

        TJsonValue obj1 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj2 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj3 = TObject{{"a", 1}, {"b", false}};
        assert(obj1 == obj2);
        assert(obj1 != obj3);
    }

    void test_deep_nesting() {
        TObject inner_obj;
        inner_obj["key"] = "value";

        TArray inner_arr;
        inner_arr.emplace_back(inner_obj);
        inner_arr.emplace_back(42);

        TObject root_obj;
        root_obj["data"] = inner_arr;

        TJsonValue root_json(root_obj);
        TJsonValue root_copy(root_json);
        
        assert(root_copy == root_json);

        // Mutate deep copy
        root_copy.get_object()["data"].get_array()[0].get_object()["key"] = "NEW_VALUE";

        // Assert divergence
        assert(root_copy != root_json);
        assert(root_json.get_object()["data"].get_array()[0].get_object()["key"].get_string() == "value");
    }

    // ИСПРАВЛЕНО: проверяем встроенные исключения в стандартных геттерах
    void test_getters_and_exceptions() {
        TJsonValue i = 42;
        TJsonValue s = "string";

        // Success paths
        assert(i.get_integer() == 42);
        assert(s.get_string() == "string");

        // Failure paths - Integer
        bool thrown = false;
        try { i.get_string(); } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);

        // Failure paths - String
        thrown = false;
        try { s.get_array(); } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);

        // Failure paths - Object
        thrown = false;
        try { i.get_object(); } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);
    }

    // ИСПРАВЛЕНО: использование актуальных методов
    void test_const_correctness() {
        const TJsonValue const_val = TArray{1, 2, 3};
        
        // Should compile and run using const getters
        assert(const_val.is_array());
        assert(const_val.get_array().size() == 3);
        assert(const_val.get_array()[0].get_integer() == 1);
        
        assert(const_val.get_array()[1].get_integer() == 2);
    }

    void test_unique_ownership_copy() {
        TJsonValue original_arr = TArray{TJsonValue(1), TJsonValue("test")};
        
        TJsonValue copied_arr = original_arr;

        const auto* orig_ptr = &original_arr.get_array()[0];
        const auto* copy_ptr = &copied_arr.get_array()[0];
        assert(orig_ptr != copy_ptr && "Deep copy failed: arrays share the same memory!");

        const std::string& orig_str = original_arr.get_array()[1].get_string();
        const std::string& copy_str = copied_arr.get_array()[1].get_string();
        assert(&orig_str != &copy_str && "Deep copy failed: strings share the same memory!");

        TJsonValue original_obj = TObject{{"key", TJsonValue(TArray{1, 2, 3})}};
        TJsonValue copied_obj = original_obj;

        const auto* orig_nested_arr = &original_obj.get_object().at("key").get_array();
        const auto* copy_nested_arr = &copied_obj.get_object().at("key").get_array();
        
        assert(orig_nested_arr != copy_nested_arr && "Deep copy failed: nested arrays share memory!");
    }

    void test_unique_ownership_move() {
        TJsonValue original_arr = TArray{TJsonValue(10), TJsonValue(20)};
        
        const auto* memory_address_before = &original_arr.get_array()[0];

        TJsonValue moved_arr = std::move(original_arr);

        const auto* memory_address_after = &moved_arr.get_array()[0];
        assert(memory_address_before == memory_address_after && "Move semantics failed: unnecessary reallocation occurred!");

        assert(original_arr.is_null() && "Move semantics failed: original object still holds data!");
        assert(original_arr.get_value_type() == EJsonType::Null);
    }

    void test_reassignment_cleanup() {
        TJsonValue val = TArray{1, 2, 3, 4, 5};
        assert(val.is_array());
        
        val = TObject{{"key", "value"}};
        assert(val.is_object() && !val.is_array());
        assert(val.get_object().size() == 1);

        val = "now i am a string";
        assert(val.is_string() && !val.is_object());

        val = 42;
        assert(val.is_integer());

        val = TNull{};
        assert(val.is_null());
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

        assert(val1.is_integer() && val1.get_integer() == 10);
        assert(val2.is_integer() && val2.get_integer() == 20);
        assert(val3.is_integer() && val3.get_integer() == 30);
        assert(val4.is_integer() && val4.get_integer() == 40);

        float f = 3.14f;
        TJsonValue val5 = f;
        assert(val5.is_double());
    }

    void test_edge_cases() {
        TJsonValue empty_arr1 = TArray{};
        TJsonValue empty_arr2 = TArray{};
        assert(empty_arr1 == empty_arr2);

        TJsonValue empty_obj1 = TObject{};
        TJsonValue empty_obj2 = TObject{};
        assert(empty_obj1 == empty_obj2);

        assert(empty_arr1 != empty_obj1);

        TJsonValue null_val;
        assert(empty_arr1 != null_val);

        TJsonValue empty_str = "";
        assert(empty_str.is_string() && empty_str.get_string().empty());
    }

    // ДОПОЛНЕНО: Тщательное покрытие математических операций и их исключений
    void test_math_operations() {
        // --- Целочисленная математика ---
        TJsonValue i = 10;
        i += 5;   assert(i.get_integer() == 15);
        i -= 2;   assert(i.get_integer() == 13);
        i *= 2;   assert(i.get_integer() == 26);
        i /= 2;   assert(i.get_integer() == 13);

        // --- Вещественная математика ---
        TJsonValue d = 2.5;
        d += 1.5; assert(d.get_double() == 4.0);
        d -= 1.0; assert(d.get_double() == 3.0);
        d *= 3.0; assert(d.get_double() == 9.0);
        d /= 2.0; assert(d.get_double() == 4.5);

        // --- Конкатенация строк ---
        TJsonValue s = "hello";
        s += TString(" world");
        assert(s.get_string() == "hello world");

        // --- Исключения: применение математики к неподходящему типу ---
        bool thrown = false;
        try { s += 5; } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);

        thrown = false;
        try { s -= 5; } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);

        thrown = false;
        try { s *= 5; } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);

        thrown = false;
        try { s /= 5; } catch (const NError::TAccessError&) { thrown = true; }
        assert(thrown);
    }

    void test_array_access() {
        TJsonValue arr = TArray{TJsonValue(10), TJsonValue(20), TJsonValue(30)};

        // Неконстантный доступ
        assert(arr[0].get_integer() == 10);
        arr[1] = 99; // Мутация через operator[]
        assert(arr.at(1).get_integer() == 99);

        // Константный доступ
        const TJsonValue const_arr = arr;
        assert(const_arr[2].get_integer() == 30);
        assert(const_arr.at(2).get_integer() == 30);

        // Проверка std::out_of_range (при использовании at())
        bool out_of_range_thrown = false;
        try {
            arr.at(100); 
        } catch (const std::out_of_range&) {
            out_of_range_thrown = true;
        }
        assert(out_of_range_thrown);

        // Проверка TAccessError, если применить [] не к массиву
        TJsonValue num = 42;
        bool access_error_thrown = false;
        try {
            num[0];
        } catch (const NError::TAccessError&) {
            access_error_thrown = true;
        }
        assert(access_error_thrown);
    }

    void test_object_access() {
        TJsonValue obj = TObject{{"name", TJsonValue("Alice")}, {"age", TJsonValue(25)}};

        // Проверка метода contains
        assert(obj.contains("name") == true);
        assert(obj.contains("height") == false);

        // Чтение через at() и []
        assert(obj["name"].get_string() == "Alice");
        assert(obj.at("age").get_integer() == 25);

        // Запись через неконстантный operator[] (создание нового ключа)
        obj["height"] = 170.5;
        assert(obj.contains("height") == true);
        assert(obj.at("height").get_double() == 170.5);

        // Константный доступ
        const TJsonValue const_obj = obj;
        assert(const_obj.at("name").get_string() == "Alice");
        
        // Проверка std::out_of_range (выбрасывается std::map::at)
        bool out_of_range_thrown = false;
        try {
            const_obj.at("non_existent_key");
        } catch (const std::out_of_range&) {
            out_of_range_thrown = true;
        }
        assert(out_of_range_thrown);

        // Проверка TAccessError, если применить ["key"] не к объекту
        TJsonValue str = "im not an object";
        bool access_error_thrown = false;
        try {
            str["key"];
        } catch (const NError::TAccessError&) {
            access_error_thrown = true;
        }
        assert(access_error_thrown);
    }
    
} // namespace NJson::NTests

int main() { 
    using namespace NJson::NTests;

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
    test_reassignment_cleanup();
    test_implicit_numeric_conversions();
    test_edge_cases();

    test_math_operations();
    test_array_access();
    test_object_access();

    std::cout << "All TJsonValue tests passed successfully! You are breathtaking!" << std::endl;
    return 0;
}
