#include <json/json_value.h>

#include <cassert>
#include <iostream>

using namespace NJson;

void check_global() {

    {
        TJsonValue json_value;
        assert(std::holds_alternative<std::monostate>(json_value.get_root_value()));
    }

    {
        TJsonValue json_value(false);
        assert(std::holds_alternative<TBoolean>(json_value.get_root_value()));       
    }

    {
        TArray array;
        array.emplace_back(false);
        array.emplace_back(1.4);
        array.emplace_back("hello");
        array.emplace_back(TString("world"));

        assert(std::holds_alternative<TBoolean>(array[0].get_root_value()));
        assert(std::holds_alternative<TNumber>(array[1].get_root_value()));
        assert(std::holds_alternative<TString>(array[2].get_root_value()));
        assert(std::holds_alternative<TString>(array[3].get_root_value()));

        TJsonValue json_value(array);
        assert(std::holds_alternative<TArrayPtr>(json_value.get_root_value()));  
        
        const TArrayPtr& cptr_array = std::get<TArrayPtr>(json_value.get_root_value());
        assert(cptr_array != nullptr);
        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::holds_alternative<TNumber>((*cptr_array)[1].get_root_value()));
        assert(std::holds_alternative<TString>((*cptr_array)[2].get_root_value()));
        assert(std::holds_alternative<TString>((*cptr_array)[3].get_root_value()));


        array[0] = 153.12;

        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::get<TBoolean>((*cptr_array)[0].get_root_value()) == false);


        TJsonValue second_json_value(json_value);
        assert(json_value == second_json_value);

        assert(std::holds_alternative<TArrayPtr>(second_json_value.get_root_value()));
        (*std::get<TArrayPtr>(second_json_value.get_root_value()))[0] = "hello";

        assert(json_value != second_json_value);
        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::get<TBoolean>((*cptr_array)[0].get_root_value()) == false);


        json_value = second_json_value;
        assert(json_value == second_json_value);
    }

    {
        TJsonValue first(TArray{});
        TJsonValue second(std::move(first));
        assert(std::holds_alternative<TNull>(first.get_root_value()));
        TJsonValue third(first);
        assert(first == third);
    }
}

void test_primitives() {
    // TNull
    TJsonValue null_val;
    TJsonValue null_val2(TNull{});
    assert(std::holds_alternative<TNull>(null_val.get_root_value()));
    assert(null_val == null_val2);

    // TInteger
    TJsonValue int_val(static_cast<TInteger>(42));
    assert(std::holds_alternative<TNumber>(int_val.get_root_value()));
    assert(std::get<TInteger>(std::get<TNumber>(int_val.get_root_value())) == 42);

    // TDouble
    TJsonValue double_val(3.14);
    assert(std::holds_alternative<TNumber>(double_val.get_root_value()));
    assert(std::get<TDouble>(std::get<TNumber>(double_val.get_root_value())) == 3.14);

    // TBoolean
    TJsonValue bool_val(true);
    assert(std::holds_alternative<TBoolean>(bool_val.get_root_value()));
    assert(std::get<TBoolean>(bool_val.get_root_value()) == true);

    // TString
    TJsonValue string_val("test_string");
    assert(std::holds_alternative<TString>(string_val.get_root_value()));
    assert(std::get<TString>(string_val.get_root_value()) == "test_string");

    // Проверка сравнения разных типов (должны быть не равны)
    assert(int_val != double_val);
    assert(int_val != string_val);
    assert(null_val != bool_val);
}

void test_arrays() {
    TArray arr;
    arr.emplace_back(10);
    arr.emplace_back("hello");
    
    TJsonValue json_arr(arr);
    assert(std::holds_alternative<TArrayPtr>(json_arr.get_root_value()));

    // Проверка глубокого копирования
    TJsonValue copy_arr(json_arr);
    assert(copy_arr == json_arr);

    // Модификация копии не должна влиять на оригинал
    auto& copy_ptr = std::get<TArrayPtr>(copy_arr.get_root_value());
    (*copy_ptr)[0] = 99;
    
    assert(copy_arr != json_arr);
    
    auto& orig_ptr = std::get<TArrayPtr>(json_arr.get_root_value());
    assert(std::get<TInteger>(std::get<TNumber>((*orig_ptr)[0].get_root_value())) == 10);
}

void test_objects() {
    TObject obj;
    obj["name"] = TJsonValue("Alice");
    obj["age"] = TJsonValue(static_cast<TInteger>(30));
    obj["is_admin"] = TJsonValue(true);

    TJsonValue json_obj(obj);
    assert(std::holds_alternative<TObjectPtr>(json_obj.get_root_value()));

    // Проверка глубокого копирования
    TJsonValue copy_obj(json_obj);
    assert(copy_obj == json_obj);

    // Модификация копии
    auto& copy_ptr = std::get<TObjectPtr>(copy_obj.get_root_value());
    (*copy_ptr)["age"] = TJsonValue(static_cast<TInteger>(31));
    
    assert(copy_obj != json_obj);
    assert(json_obj != copy_obj);
}

void test_deep_nesting() {
    // Создаем сложную структуру: объект, внутри которого массив, внутри которого другой объект
    TObject inner_obj;
    inner_obj["key"] = TJsonValue("value");

    TArray inner_arr;
    inner_arr.emplace_back(inner_obj);
    inner_arr.emplace_back(42);

    TObject root_obj;
    root_obj["data"] = TJsonValue(inner_arr);

    TJsonValue root_json(root_obj);

    // Копируем всю эту матрешку
    TJsonValue root_copy(root_json);
    assert(root_copy == root_json);

    // Меняем самое глубокое значение в копии
    auto& copy_root_ptr = std::get<TObjectPtr>(root_copy.get_root_value());
    auto& copy_arr_val = (*copy_root_ptr)["data"];
    auto& copy_arr_ptr = std::get<TArrayPtr>(copy_arr_val.get_root_value());
    auto& copy_inner_obj_val = (*copy_arr_ptr)[0];
    auto& copy_inner_obj_ptr = std::get<TObjectPtr>(copy_inner_obj_val.get_root_value());
    
    (*copy_inner_obj_ptr)["key"] = TJsonValue("NEW_VALUE");

    // Оригинал и копия больше не равны
    assert(root_copy != root_json);
}

void test_move_semantics() {
    // Move-конструктор
    TJsonValue original_str("movable_string");
    TJsonValue moved_to(std::move(original_str));
    
    assert(std::holds_alternative<TString>(moved_to.get_root_value()));
    assert(std::holds_alternative<TNull>(original_str.get_root_value())); // Проверка работы clear()

    // Move-оператор присваивания
    TJsonValue obj_val(TObject{});
    TJsonValue target_val;
    target_val = std::move(obj_val);

    assert(std::holds_alternative<TObjectPtr>(target_val.get_root_value()));
    assert(std::holds_alternative<TNull>(obj_val.get_root_value())); // Проверка работы clear()

    // Сравнение перемещенного объекта с null
    TJsonValue null_val;
    assert(obj_val == null_val);
}

void test_assignments() {
    TJsonValue val;
    
    val = static_cast<TInteger>(100);
    assert(std::holds_alternative<TNumber>(val.get_root_value()));
    
    val = "reassigned";
    assert(std::holds_alternative<TString>(val.get_root_value()));
    
    val = TNull{};
    assert(std::holds_alternative<TNull>(val.get_root_value()));
}

int main() { 
    check_global();
    test_primitives();
    test_arrays();
    test_objects();
    test_deep_nesting();
    test_move_semantics();
    test_assignments();

    std::cout << "All TJsonValue tests passed successfully!" << std::endl;
    return 0;
}
