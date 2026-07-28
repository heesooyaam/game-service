#include <library/json/json_serializer.h>
#include <library/json/json_value.h>
#include <library/json/json_parser.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>

namespace NJson::NTests {

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

    // Вспомогательная функция для удобной сериализации в строку
    std::string serialize_to_string(const TJsonValue& json_value) {
        std::ostringstream oss;
        TJsonSerializer serializer(json_value, oss);
        serializer.serialize(); // или .serialize() в зависимости от API
        std::cout << oss.str() << std::endl;
        return oss.str();
    }

    void test_serializes_null() {
        TJsonValue json_null; // или TJsonValue::Null() в зависимости от твоего API
        check(serialize_to_string(json_null) == "null");
    }

    void test_serializes_integer() {
        TJsonValue json_int(42);
        check(serialize_to_string(json_int) == "42");
        
        TJsonValue json_negative_int(-100);
        check(serialize_to_string(json_negative_int) == "-100");
    }

    void test_serializes_double() {
        TJsonValue json_double(3.1415);
        // Сравниваем с std::to_string, так как в твоем коде используется именно он
        std::string expected = std::to_string(3.1415);
        check(serialize_to_string(json_double) == expected);
    }

    void test_serializes_boolean() {
        TJsonValue json_true(true);
        check(serialize_to_string(json_true) == "true");

        TJsonValue json_false(false);
        check(serialize_to_string(json_false) == "false");
    }

    void test_serializes_string() {
        TJsonValue json_str("hello world");
        check(json_str.is_string());
        check(serialize_to_string(json_str) == "\"hello world\"");
    }

    void test_serializes_empty_array() {
        TJsonValue json_array = TArray(); // адаптация под твой способ создания пустого массива
        check(serialize_to_string(json_array) == "[]");
    }

    void test_serializes_array_with_elements() {
        TArray arr;
        arr.push_back(TJsonValue(1));
        arr.push_back(TJsonValue("two"));
        arr.push_back(TJsonValue(false));
        TJsonValue json_array(arr);

        check(serialize_to_string(json_array) == "[1,\"two\",false]");
    }

    void test_serializes_empty_object() {
        TJsonValue json_obj = TObject();
        check(serialize_to_string(json_obj) == "{}");
    }

    void test_serializes_object_with_elements() {
        TObject obj;
        obj["key1"] = TJsonValue("value1");
        obj["key2"] = TJsonValue(123);
        TJsonValue json_obj(obj);

        std::string result = serialize_to_string(json_obj);
        // Проверяем оба варианта порядка ключей, так как хэш-таблицы не гарантируют порядок
        bool is_ok = (result == "{\"key1\":\"value1\",\"key2\":123}" || 
                      result == "{\"key2\":123,\"key1\":\"value1\"}");
        check(is_ok);
    }

    void test_serializes_nested_structures() {
        TArray inner_arr;
        inner_arr.push_back(TJsonValue(1));
        inner_arr.push_back(TJsonValue(2));

        TObject obj;
        obj["name"] = TJsonValue("test");
        obj["data"] = TJsonValue(inner_arr);
        
        TJsonValue json_doc(obj);

        std::string result = serialize_to_string(json_doc);
        
        check(result.find("\"name\":\"test\"") != std::string::npos);
        check(result.find("\"data\":[1,2]") != std::string::npos);
        check(result.front() == '{');
        check(result.back() == '}');
    }

    void test_parser_serializer() {
        TArray inner_arr;
        inner_arr.push_back(TJsonValue(1));
        inner_arr.push_back(TJsonValue(2));

        TObject obj;
        obj["name"] = TJsonValue("test");
        obj["data"] = TJsonValue(inner_arr);
        
        TJsonValue json_doc(obj);

        auto val = parse(serialize_to_string(json_doc));
        assert(val == json_doc);
    }

    void test_parser_serializer_null() {
        TJsonValue json_null; // Инициализация null по умолчанию
        auto val = parse(serialize_to_string(json_null));
        assert(val == json_null);
    }

    // 2. Тесты для булевых значений
    void test_parser_serializer_boolean() {
        TJsonValue json_true(true);
        assert(parse(serialize_to_string(json_true)) == json_true);

        TJsonValue json_false(false);
        assert(parse(serialize_to_string(json_false)) == json_false);
    }

    // 3. Тесты для чисел (целые и с плавающей точкой)
    void test_parser_serializer_numbers() {
        TJsonValue json_int(42);
        assert(parse(serialize_to_string(json_int)) == json_int);

        TJsonValue json_negative(-999);
        assert(parse(serialize_to_string(json_negative)) == json_negative);

        // Для double лучше использовать числа, которые точно представляются в двоичном виде,
        // чтобы избежать проблем с точностью при строгом сравнении через ==
        TJsonValue json_double(3.5); 
        assert(parse(serialize_to_string(json_double)) == json_double);
    }

    // 4. Тесты для строк (включая пустую строку)
    void test_parser_serializer_strings() {
        TJsonValue json_str("hello world");
        assert(parse(serialize_to_string(json_str)) == json_str);

        TJsonValue json_empty_str("");
        assert(parse(serialize_to_string(json_empty_str)) == json_empty_str);

        // Строка со спецсимволами (если ваш парсер поддерживает экранирование)
        TJsonValue json_escaped(R"(line1\nline2\t\"quoted\")");
        assert(parse(serialize_to_string(json_escaped)) == "line1\nline2\t\"quoted\"");
    }

    // 5. Тесты для пустых коллекций
    void test_parser_serializer_empty_structures() {
        TJsonValue json_empty_arr((TArray())); // Скобки нужны, чтобы избежать vexing parse
        assert(parse(serialize_to_string(json_empty_arr)) == json_empty_arr);

        TJsonValue json_empty_obj((TObject()));
        assert(parse(serialize_to_string(json_empty_obj)) == json_empty_obj);
    }

    // 6. Тест для массива с разными типами данных (Mixed Array)
    void test_parser_serializer_mixed_array() {
        TArray arr;
        arr.push_back(TJsonValue("string"));
        arr.push_back(TJsonValue(100));
        arr.push_back(TJsonValue(false));
        arr.push_back(TJsonValue()); // null
        
        TJsonValue json_doc(arr);
        auto val = parse(serialize_to_string(json_doc));
        assert(val == json_doc);
    }

    // 7. Тест для глубоко вложенных объектов (Deeply Nested)
    void test_parser_serializer_deep_nesting() {
        TObject level_3;
        level_3["target"] = TJsonValue("found me");

        TObject level_2;
        level_2["next"] = TJsonValue(level_3);

        TObject level_1;
        level_1["nested_obj"] = TJsonValue(level_2);
        
        // Добавим еще и массив внутрь корневого объекта
        TArray arr;
        arr.push_back(TJsonValue(1));
        level_1["numbers"] = TJsonValue(arr);

        TJsonValue json_doc(level_1);

        auto val = parse(serialize_to_string(json_doc));
        assert(val == json_doc);
    }


    // 1. Тест простого значения (число)
    void test_ostream_operator_simple() {
        TJsonValue json_int(42);
        std::ostringstream oss;
        
        oss << json_int; // Вызов нашего оператора
        
        check(oss.str() == "42");
    }

    // 2. Тест сложной структуры (объект)
    void test_ostream_operator_complex() {
        TObject obj;
        obj["key"] = TJsonValue("value");
        TJsonValue json_obj(obj);

        std::ostringstream oss;
        oss << json_obj;
        
        check(oss.str() == "{\"key\":\"value\"}");
    }

    // 3. Тест цепочки вызовов (Chaining) — самое важное для operator<<
    void test_ostream_operator_chaining() {
        TJsonValue json_true(true);
        std::ostringstream oss;
        
        // Проверяем, что оператор возвращает TOstream& и позволяет продолжать вывод
        oss << "JSON result is: " << json_true << " (end)";
        
        check(oss.str() == "JSON result is: true (end)");
    }

    // 4. Тест с другим типом потока (std::stringstream), 
    // чтобы убедиться, что шаблон <typename TOstream> работает корректно
    void test_ostream_operator_template_resolution() {
        TJsonValue json_null; // Предполагается, что по умолчанию это null
        std::stringstream ss; // Используем stringstream вместо ostringstream
        
        ss << json_null;
        
        check(ss.str() == "null");
    }

    std::string serialize_to_string_normal(const TJsonValue& json_value) {
        std::ostringstream oss;
        TJsonSerializer serializer(json_value, oss);
        serializer.serialize_normal_mode();
        return oss.str();
    }

    void test_normal_mode_primitives() {
        // Примитивы сериализуются одинаково в обоих режимах
        check(serialize_to_string_normal(TJsonValue()) == "null");
        check(serialize_to_string_normal(TJsonValue(42)) == "42");
        check(serialize_to_string_normal(TJsonValue(true)) == "true");
        check(serialize_to_string_normal(TJsonValue("test")) == "\"test\"");
    }

    void test_normal_mode_array() {
        TArray arr;
        arr.push_back(TJsonValue(1));
        arr.push_back(TJsonValue(2));
        arr.push_back(TJsonValue(3));
        
        // В normal_mode после запятой должен быть пробел
        check(serialize_to_string_normal(TJsonValue(arr)) == "[1, 2, 3]");
    }

    void test_normal_mode_empty_object() {
        // Пустой объект должен остаться {} без переносов
        check(serialize_to_string_normal(TJsonValue(TObject())) == "{}");
    }

    void test_normal_mode_object() {
        TObject obj;
        obj["key"] = TJsonValue(42);
        
        std::string expected = 
            "{\n"
            "    \"key\" : 42\n"
            "}";
            
        check(serialize_to_string_normal(TJsonValue(obj)) == expected);
    }

    void test_normal_mode_nested_object() {
        // Проверяем увеличение отступа (indent += 4) для вложенных структур
        TObject inner_obj;
        inner_obj["inner_key"] = TJsonValue("value");
        
        TObject outer_obj;
        outer_obj["outer_key"] = TJsonValue(inner_obj);
        
        std::string expected = 
            "{\n"
            "    \"outer_key\" : {\n"
            "        \"inner_key\" : \"value\"\n"
            "    }\n"
            "}";
            
        check(serialize_to_string_normal(TJsonValue(outer_obj)) == expected);
    }

    void test_normal_mode_object_with_array() {
        TArray arr;
        arr.push_back(TJsonValue(1));
        arr.push_back(TJsonValue(2));

        TObject obj;
        obj["arr"] = TJsonValue(arr);

        std::string expected = 
            "{\n"
            "    \"arr\" : [1, 2]\n"
            "}";
            
        check(serialize_to_string_normal(TJsonValue(obj)) == expected);
    }

    void test_parser_serializer_null_normal() {
        TJsonValue json_null; // Инициализация null по умолчанию
        auto val = parse(serialize_to_string_normal(json_null));
        assert(val == json_null);
    }

    // 2. Тесты для булевых значений
    void test_parser_serializer_boolean_normal() {
        TJsonValue json_true(true);
        assert(parse(serialize_to_string_normal(json_true)) == json_true);

        TJsonValue json_false(false);
        assert(parse(serialize_to_string_normal(json_false)) == json_false);
    }

    // 3. Тесты для чисел (целые и с плавающей точкой)
    void test_parser_serializer_numbers_normal() {
        TJsonValue json_int(42);
        assert(parse(serialize_to_string_normal(json_int)) == json_int);

        TJsonValue json_negative(-999);
        assert(parse(serialize_to_string_normal(json_negative)) == json_negative);

        // Для double лучше использовать числа, которые точно представляются в двоичном виде,
        // чтобы избежать проблем с точностью при строгом сравнении через ==
        TJsonValue json_double(3.5); 
        assert(parse(serialize_to_string_normal(json_double)) == json_double);
    }

    // 4. Тесты для строк (включая пустую строку)
    void test_parser_serializer_strings_normal() {
        TJsonValue json_str("hello world");
        assert(parse(serialize_to_string_normal(json_str)) == json_str);

        TJsonValue json_empty_str("");
        assert(parse(serialize_to_string_normal(json_empty_str)) == json_empty_str);

        // Строка со спецсимволами (если ваш парсер поддерживает экранирование)
        TJsonValue json_escaped(R"(line1\nline2\t\"quoted\")");
        assert(parse(serialize_to_string_normal(json_escaped)) == "line1\nline2\t\"quoted\"");
    }

    // 5. Тесты для пустых коллекций
    void test_parser_serializer_empty_structures_normal() {
        TJsonValue json_empty_arr((TArray())); // Скобки нужны, чтобы избежать vexing parse
        assert(parse(serialize_to_string_normal(json_empty_arr)) == json_empty_arr);

        TJsonValue json_empty_obj((TObject()));
        assert(parse(serialize_to_string_normal(json_empty_obj)) == json_empty_obj);
    }

    // 6. Тест для массива с разными типами данных (Mixed Array)
    void test_parser_serializer_mixed_array_normal() {
        TArray arr;
        arr.push_back(TJsonValue("string"));
        arr.push_back(TJsonValue(100));
        arr.push_back(TJsonValue(false));
        arr.push_back(TJsonValue()); // null
        
        TJsonValue json_doc(arr);
        auto val = parse(serialize_to_string_normal(json_doc));
        assert(val == json_doc);
    }

    // 7. Тест для глубоко вложенных объектов (Deeply Nested)
    void test_parser_serializer_deep_nesting_normal() {
        TObject level_3;
        level_3["target"] = TJsonValue("found me");

        TObject level_2;
        level_2["next"] = TJsonValue(level_3);

        TObject level_1;
        level_1["nested_obj"] = TJsonValue(level_2);
        
        // Добавим еще и массив внутрь корневого объекта
        TArray arr;
        arr.push_back(TJsonValue(1));
        level_1["numbers"] = TJsonValue(arr);

        TJsonValue json_doc(level_1);

        auto val = parse(serialize_to_string_normal(json_doc));
        assert(val == json_doc);
    }
    

} //namespace NJson::NTests

int main() {
    using namespace NJson::NTests;

    test_serializes_null();
    test_serializes_integer();
    test_serializes_double();
    test_serializes_boolean();
    test_serializes_string();
    test_serializes_empty_array();
    test_serializes_array_with_elements();
    test_serializes_empty_object();
    test_serializes_object_with_elements();
    test_serializes_nested_structures();
    test_parser_serializer();

    test_parser_serializer_null();
    test_parser_serializer_boolean();
    test_parser_serializer_numbers();
    test_parser_serializer_strings();
    test_parser_serializer_empty_structures();
    test_parser_serializer_mixed_array();
    test_parser_serializer_deep_nesting();

    test_ostream_operator_simple();
    test_ostream_operator_complex();
    test_ostream_operator_chaining();
    test_ostream_operator_template_resolution();
        
    // Запуск тестов для normal_mode
    test_normal_mode_primitives();
    test_normal_mode_array();
    test_normal_mode_empty_object();
    test_normal_mode_object();
    test_normal_mode_nested_object();
    test_normal_mode_object_with_array();

    test_parser_serializer_null_normal();
    test_parser_serializer_boolean_normal();
    test_parser_serializer_numbers_normal();
    test_parser_serializer_strings_normal();
    test_parser_serializer_empty_structures_normal();
    test_parser_serializer_mixed_array_normal();
    test_parser_serializer_deep_nesting_normal();
        
    std::cout << "All JSON serializer tests passed successfully!\n";
}
