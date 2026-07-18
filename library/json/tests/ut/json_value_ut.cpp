#include <library/json/json_value.h>
#include <library/json/error.h> 

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
        try { arr.at(100); } catch (const std::out_of_range&) { thrown = true; }
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
        try { const_obj.at("non_existent_key"); } catch (const std::out_of_range&) { thrown = true; }
        assert(thrown);

        TJsonValue str = "string";
        thrown = false;
        try { str["key"]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_path_access() {
        TObject root_obj;
        TArray arr;
        arr.emplace_back(10);
        arr.emplace_back(20);
        TObject inner;
        inner["cheburek"] = "tasty";
        arr.emplace_back(std::move(inner));
        
        TObject lol;
        lol["kek"] = std::move(arr);
        root_obj["lol"] = std::move(lol);
        
        TJsonValue root(std::move(root_obj));

        // 1. Успешное чтение (non-const)
        assert(root.get_value_by_path("lol/kek/0").get_integer() == 10);
        assert(root.get_value_by_path("lol/kek/2/cheburek").get_string() == "tasty");

        // 2. Успешное чтение (const)
        const TJsonValue const_root = root;
        assert(const_root.get_value_by_path("lol/kek/1").get_integer() == 20);
        assert(const_root.get_value_by_path("lol/kek/2/cheburek").get_string() == "tasty");

        // 3. Исключения: выход за границы массива
        bool thrown = false;
        try { const_root.get_value_by_path("lol/kek/99"); } 
        catch (const NError::TJsonBadArrayIndex&) { thrown = true; }
        assert(thrown);

        // 4. Исключения: несуществующий ключ
        thrown = false;
        try { const_root.get_value_by_path("lol/kek/2/non_existent"); } 
        catch (const NError::TJsonBadObjectKey&) { thrown = true; }
        assert(thrown);

        // 5. Исключения: попытка читать массив как объект (передали строку вместо индекса)
        thrown = false;
        try { const_root.get_value_by_path("lol/kek/not_a_number"); } 
        // parse_array_index вернет nullopt -> уйдет в ветку объектов -> !is_object() бросит TJsonTypeError
        catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }

    void test_get_and_create_value_by_path() {
        TJsonValue root = TObject{};
        
        // 1. Магическое авто-создание пути (Auto-vivification)
        root.get_and_create_value_by_path("settings/graphics/resolution") = "1080p";
        assert(root.get_value_by_path("settings/graphics/resolution").get_string() == "1080p");
        
        // Добавление ключа в уже существующий объект
        root.get_and_create_value_by_path("settings/audio") = 100;
        assert(root.get_value_by_path("settings/audio").get_integer() == 100);

        // 2. Исключение: Нельзя магически создать элемент массива по индексу (только ключи объектов)
        root.get_and_create_value_by_path("arr_parent") = TArray{1, 2, 3};
        bool thrown = false;
        try { root.get_and_create_value_by_path("arr_parent/99"); }
        catch (const NError::TJsonBadArrayIndex&) { thrown = true; }
        assert(thrown);

        // 3. Исключение: Обращение к числу/строке как к объекту
        thrown = false;
        try { root.get_and_create_value_by_path("settings/audio/volume"); } 
        // "audio" это int. nullopt -> else -> is_null()==false, is_object()==false -> throw TJsonTypeError
        catch (const NError::TJsonTypeError&) { thrown = true; }
        assert(thrown);
    }


    void test_exception_text() {
        TJsonValue json = TObject{};
        json.get_and_create_value_by_path("settings/audio/volume") = 100;
        
        // Создаем константную ссылку для проверок const-методов
        const TJsonValue& const_json = json;

        // =====================================================================
        // 1. Проверка TJsonBadObjectKey
        // =====================================================================
        
        bool thrown = false;
        try {
            auto value = json.get_value_by_path("settings/volume");
        } catch (const NError::TJsonBadObjectKey& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD OBJECT KEY]: volume");
        }
        assert(thrown);

        thrown = false;
        try {
            auto value = const_json.get_value_by_path("settings/volume");
        } catch (const NError::TJsonBadObjectKey& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD OBJECT KEY]: volume");
        }
        assert(thrown);


        // =====================================================================
        // 2. Проверка TJsonTypeError (Ожидали Array, получили Object)
        // =====================================================================
        
        thrown = false;
        try {
            // "settings" это объект, но мы обращаемся к нему по индексу "0"
            auto value = json.get_value_by_path("settings/0");
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected ARRAY, but got OBJECT");
        }
        assert(thrown);

        thrown = false;
        try {
            auto value = const_json.get_value_by_path("settings/0");
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected ARRAY, but got OBJECT");
        }
        assert(thrown);

        thrown = false;
        try {
            json.get_and_create_value_by_path("settings/0/new_key");
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected ARRAY, but got OBJECT");
        }
        assert(thrown);


        // =====================================================================
        // 3. Проверка TJsonTypeError (Ожидали Object, получили Integer)
        // =====================================================================
        
        TJsonValue arr_json = TArray{TJsonValue(1), TJsonValue(2), TJsonValue(3)};
        const TJsonValue& const_arr_json = arr_json;

        thrown = false;
        try {
            // Обращаемся к элементу массива (числу 2) как к объекту
            auto value = arr_json.get_value_by_path("1/settings");
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected OBJECT, but got INTEGER");
        }
        assert(thrown);

        thrown = false;
        try {
            auto value = const_arr_json.get_value_by_path("1/settings");
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected OBJECT, but got INTEGER");
        }
        assert(thrown);

        thrown = false;
        try {
            arr_json.get_and_create_value_by_path("1/settings/volume") = 50;
        } catch (const NError::TJsonTypeError& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ACCESS ERROR]: Expected OBJECT, but got INTEGER");
        }
        assert(thrown);


        // =====================================================================
        // 4. Проверка TJsonBadArrayIndex
        // =====================================================================
        
        thrown = false;
        try {
            // Массив размером 3, индекс 5 выходит за пределы
            auto value = arr_json.get_value_by_path("5");
        } catch (const NError::TJsonBadArrayIndex& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ARRAY INDEX]: array size = 3, index = 5");
        }
        assert(thrown);

        thrown = false;
        try {
            auto value = const_arr_json.get_value_by_path("5");
        } catch (const NError::TJsonBadArrayIndex& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ARRAY INDEX]: array size = 3, index = 5");
        }
        assert(thrown);

        thrown = false;
        try {
            // При использовании get_and_create_value_by_path массив тоже не должен расширяться сам
            arr_json.get_and_create_value_by_path("5/new_key") = 100;
        } catch (const NError::TJsonBadArrayIndex& exp) {
            thrown = true;
            assert(std::string_view(exp.what()) == "[BAD ARRAY INDEX]: array size = 3, index = 5");
        }
        assert(thrown);
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
