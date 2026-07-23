#include <library/json/json_value.h>
#include <library/json/error.h>

#include <cassert>
#include <iostream>

namespace NJson::NTests {

    void test_concepts() {
        // 1. Тесты для CCharacter
        static_assert(CCharacter<char>);
        static_assert(CCharacter<wchar_t>);
        static_assert(CCharacter<char32_t>);
        static_assert(CCharacter<const char&>); // Проверка std::remove_cvref_t
        static_assert(CCharacter<unsigned char&&>); 
        static_assert(!CCharacter<int>);        // Отрицательный тест
        static_assert(!CCharacter<double>);     // Отрицательный тест

        // 2. Тесты для CJsonInteger
        static_assert(CJsonInteger<int>);
        static_assert(CJsonInteger<long long>);
        static_assert(CJsonInteger<std::uint64_t>);
        static_assert(CJsonInteger<short>);
        static_assert(CJsonInteger<const int&>); // Проверка std::remove_cvref_t
        // Проверка исключений:
        static_assert(!CJsonInteger<bool>);      // bool исключен явно
        static_assert(!CJsonInteger<char>);      // Символьные типы исключены
        static_assert(!CJsonInteger<double>);    // Не целочисленный тип

        // 3. Тесты для CJsonFloatingPoint
        static_assert(CJsonFloatingPoint<float>);
        static_assert(CJsonFloatingPoint<double>);
        static_assert(CJsonFloatingPoint<long double>);
        static_assert(CJsonFloatingPoint<const double&>); // Проверка std::remove_cvref_t
        static_assert(!CJsonFloatingPoint<int>);          // Отрицательный тест
        static_assert(!CJsonFloatingPoint<bool>);         // Отрицательный тест

        // 4. Тесты для CJsonNumber
        // Должен пропускать целые числа (кроме bool и char) и числа с плавающей точкой
        static_assert(CJsonNumber<int>);
        static_assert(CJsonNumber<double>);
        static_assert(CJsonNumber<float>);
        static_assert(CJsonNumber<size_t>);
        static_assert(CJsonNumber<const volatile long&&>);
        // Не должен пропускать все остальное
        static_assert(!CJsonNumber<bool>);
        static_assert(!CJsonNumber<char>);
        static_assert(!CJsonNumber<std::nullptr_t>);
        static_assert(!CJsonNumber<void*>);
    }

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
        TJsonValue arr_val(std::move(arr));
        assert(arr_val.is_array());
        assert(arr_val.size() == 2);
        assert(arr_val.get_array()[0].get_integer() == 1);

        // Object
        TObject obj;
        obj["key"] = 100;
        TJsonValue obj_val(std::move(obj));
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
        assert(val.is_array() && val.size() == 2);

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

        // Deep copy verification
        copied_assign.get_array()[0] = 99;
        assert(copied_assign != original);
        assert(original.get_array()[0].get_integer() == 1);

        // Self-assignment copy
        copied_assign = copied_assign;
        assert(copied_assign.is_array() && copied_assign.get_array()[0].get_integer() == 99);
    }

    void test_move_semantics() {
        TJsonValue original = TString("movable_string");
        
        // Move constructor
        TJsonValue moved_construct(std::move(original));
        assert(moved_construct.is_string() && moved_construct.get_string() == "movable_string");
        assert(original.is_null());

        // Move assignment
        TJsonValue original_obj = TObject{{"key", 42}};
        TJsonValue moved_assign;
        moved_assign = std::move(original_obj);
        
        assert(moved_assign.is_object() && moved_assign.get_object()["key"].get_integer() == 42);
        assert(original_obj.is_null());

        // Self-assignment move
        moved_assign = std::move(moved_assign);
        assert(moved_assign.is_object());
    }

    void test_equality() {
        assert(TJsonValue(42) == TJsonValue(42));
        assert(TJsonValue(42) != TJsonValue(43));
        assert(TJsonValue(42) != TJsonValue(42.0)); // Strict type checking

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
        inner_arr.emplace_back(std::move(inner_obj));
        inner_arr.emplace_back(42);

        TObject root_obj;
        root_obj["data"] = std::move(inner_arr);

        TJsonValue root_json(std::move(root_obj));
        TJsonValue root_copy(root_json);
        
        assert(root_copy == root_json);

        root_copy.get_object()["data"].get_array()[0].get_object()["key"] = "NEW_VALUE";

        assert(root_copy != root_json);
        assert(root_json.get_object()["data"].get_array()[0].get_object()["key"].get_string() == "value");
    }

    void test_getters_and_exceptions() {
        TJsonValue i = 42;
        TJsonValue s = "string";

        assert(i.get_integer() == 42);
        assert(s.get_string() == "string");

        bool thrown = false;
        try { i.get_string(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);

        thrown = false;
        try { s.get_array(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);

        thrown = false;
        try { i.get_object(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_const_correctness() {
        const TJsonValue const_val = TArray{1, 2, 3};
        
        assert(const_val.is_array());
        assert(const_val.size() == 3);
        assert(const_val.get_array()[0].get_integer() == 1);
        assert(const_val.get_array()[1].get_integer() == 2);
    }

    void test_unique_ownership_copy() {
        TJsonValue original_arr = TArray{TJsonValue(1), TJsonValue("test")};
        TJsonValue copied_arr = original_arr;

        const auto* orig_ptr = &original_arr.get_array()[0];
        const auto* copy_ptr = &copied_arr.get_array()[0];
        assert(orig_ptr != copy_ptr);

        const std::string& orig_str = original_arr.get_array()[1].get_string();
        const std::string& copy_str = copied_arr.get_array()[1].get_string();
        assert(&orig_str != &copy_str);
    }

    void test_unique_ownership_move() {
        TJsonValue original_arr = TArray{TJsonValue(10), TJsonValue(20)};
        const auto* memory_address_before = &original_arr.get_array()[0];

        TJsonValue moved_arr = std::move(original_arr);
        const auto* memory_address_after = &moved_arr.get_array()[0];
        
        assert(memory_address_before == memory_address_after);
        assert(original_arr.is_null());
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

        assert(val1.get_integer() == 10);
        assert(val2.get_integer() == 20);
        assert(val3.get_integer() == 30);
        assert(val4.get_integer() == 40);

        float f = 3.14f;
        TJsonValue val5 = f;
        assert(val5.is_double());
    }

    void test_math_operations() {
        TJsonValue i = 10;
        i += 5;   assert(i.get_integer() == 15);
        i -= 2;   assert(i.get_integer() == 13);
        i *= 2;   assert(i.get_integer() == 26);
        i /= 2;   assert(i.get_integer() == 13);

        TJsonValue d = 2.5;
        d += 1.5; assert(d.get_double() == 4.0);
        d -= 1.0; assert(d.get_double() == 3.0);
        d *= 3.0; assert(d.get_double() == 9.0);
        d /= 2.0; assert(d.get_double() == 4.5);

        TJsonValue s = "hello";
        s += TString(" world");
        assert(s.get_string() == "hello world");

        bool thrown = false;
        try { s += 5; } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_array_access() {
        TJsonValue arr = TArray{TJsonValue(10), TJsonValue(20), TJsonValue(30)};

        assert(arr[0].get_integer() == 10);
        arr[1] = 99;
        assert(arr.at(1).get_integer() == 99);

        const TJsonValue const_arr = arr;
        assert(const_arr[2].get_integer() == 30);
        assert(const_arr.at(2).get_integer() == 30);

        bool thrown = false;
        try { arr.at(100); } catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        assert(thrown);

        TJsonValue num = 42;
        thrown = false;
        try { num[0]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_object_access() {
        TJsonValue obj = TObject{{"name", TJsonValue("Alice")}, {"age", TJsonValue(25)}};

        assert(obj.contains("name"));
        assert(!obj.contains("height"));

        assert(obj["name"].get_string() == "Alice");
        assert(obj.at("age").get_integer() == 25);

        obj["height"] = 170.5;
        assert(obj.at("height").get_double() == 170.5);

        const TJsonValue const_obj = obj;
        assert(const_obj.at("name").get_string() == "Alice");
        
        bool thrown = false;
        try { const_obj.at("non_existent_key"); } catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        assert(thrown);

        TJsonValue str = "string";
        thrown = false;
        try { str["key"]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
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

        // Корректный доступ по пути
        assert(json.get_value_by_path("/users/0/id").get_integer() == 1);
        assert(json.get_value_by_path("/users/1/name").get_string() == "Bob");
        assert(json.get_value_by_path("/metadata/count").get_integer() == 2);

        // Проверка константного доступа
        const TJsonValue& const_json = json;
        assert(const_json.get_value_by_path("/users/0/name").get_string() == "Alice");

        // Объект со строковым ключом, который выглядит как число
        TJsonValue dict = TObject{{"0", "zero_key_value"}};
        assert(dict.get_value_by_path("/0").get_string() == "zero_key_value");

        // Исключение: Путь не начинается со слеша
        bool thrown = false;
        try { json.get_value_by_path("users/0"); } 
        catch (const NError::TJsonBadPath&) { thrown = true; }
        assert(thrown);

        // Исключение: Выход за пределы массива
        thrown = false;
        try { json.get_value_by_path("/users/5"); } 
        catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        assert(thrown);

        // Исключение: Обращение к несуществующему ключу объекта
        thrown = false;
        try { json.get_value_by_path("/metadata/version"); } 
        catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        assert(thrown);

        // Исключение: Обращение к примитивному типу как к объекту/массиву
        thrown = false;
        try { json.get_value_by_path("/metadata/count/value"); } 
        catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_get_and_create_value_by_path() {
        TJsonValue json = TNull{};

        // Базовое создание иерархии
        json.get_and_create_value_by_path("/config/server/port") = 8080;
        assert(json.is_object());
        assert(json.get_value_by_path("/config/server/port").get_integer() == 8080);

        // Добавление в уже существующий объект
        json.get_and_create_value_by_path("/config/server/host") = "localhost";
        assert(json.get_value_by_path("/config/server/host").get_string() == "localhost");

        // Проверка того, что автоматическое создание порождает объекты, 
        // даже если ключ выглядит как индекс массива
        json.get_and_create_value_by_path("/items/0") = "first";
        assert(json.get_value_by_path("/items").is_object()); 
        assert(json.get_value_by_path("/items/0").get_string() == "first");

        // Мутация существующего массива по индексу
        json.get_and_create_value_by_path("/real_array") = TArray{TJsonValue(10), TJsonValue(20)};
        json.get_and_create_value_by_path("/real_array/1") = 99; // Должен изменить существующий элемент
        assert(json.get_value_by_path("/real_array/1").get_integer() == 99);

        // Исключение: Попытка создать путь поверх примитивного типа
        bool thrown = false;
        try { json.get_and_create_value_by_path("/config/server/port/value"); } 
        catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_exception_text() {
        bool thrown = false;
        try {
            TJsonValue val = 42;
            val.get_string();
        } catch (const NError::TJsonTypeError& e) {
            thrown = true;
            std::string msg = e.what();
            // Сравниваем, что в строке присутствуют имена типов из енума
            assert(msg.find("STRING") != std::string::npos);
            assert(msg.find("INTEGER") != std::string::npos);
        }
        assert(thrown);

        thrown = false;
        try {
            TJsonValue val = TArray{1, 2};
            val.at(5);
        } catch (const NError::TJsonArrayOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            assert(msg.find("array size = 2") != std::string::npos);
            assert(msg.find("index = 5") != std::string::npos);
        }
        assert(thrown);

        thrown = false;
        try {
            TJsonValue val = TObject{{"key", 1}};
            val.at("missing_key");
        } catch (const NError::TJsonObjectOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            assert(msg.find("[JSON OBJECT OUT OF RANGE]: missing_key") != std::string::npos);
        }
        assert(thrown);
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

    std::cout << "All TJsonValue tests passed successfully! You are breathtaking!" << std::endl;
    return 0;
}
