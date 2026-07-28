#include <library/json/json_parser.h>
#include <library/json/json_serializer.h>
#include <library/json/json_value.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

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

namespace {

    std::string serialize_to_string(const NJson::TJsonValue& json_value) {
        std::ostringstream oss;
        NJson::TJsonSerializer serializer(json_value, oss, NJson::EJsonFormat::Compact);
        serializer.serialize();
        std::cout << oss.str() << std::endl;
        return oss.str();
    }

    std::string serialize_to_string_pretty(const NJson::TJsonValue& json_value) {
        std::ostringstream oss;
        NJson::TJsonSerializer serializer(json_value, oss, NJson::EJsonFormat::Pretty);
        serializer.serialize();
        return oss.str();
    }

}

namespace NJson::NTests {

    void test_serializes_null() {
        TJsonValue json_null; 
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
        check(serialize_to_string(json_double) == "3.1415");
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
        TJsonValue json_array = TArray();
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
        check(val == json_doc);
    }

    void test_parser_serializer_null() {
        TJsonValue json_null;
        auto val = parse(serialize_to_string(json_null));
        check(val == json_null);
    }

    void test_parser_serializer_boolean() {
        TJsonValue json_true(true);
        check(parse(serialize_to_string(json_true)) == json_true);

        TJsonValue json_false(false);
        check(parse(serialize_to_string(json_false)) == json_false);
    }

    void test_parser_serializer_numbers() {
        TJsonValue json_int(42);
        check(parse(serialize_to_string(json_int)) == json_int);

        TJsonValue json_negative(-999);
        check(parse(serialize_to_string(json_negative)) == json_negative);

        TJsonValue json_double(3.5); 
        check(parse(serialize_to_string(json_double)) == json_double);
    }

    void test_parser_serializer_strings() {
        TJsonValue json_str("hello world");
        check(parse(serialize_to_string(json_str)) == json_str);

        TJsonValue json_empty_str("");
        check(parse(serialize_to_string(json_empty_str)) == json_empty_str);

        TJsonValue json_escaped("line1\nline2\t\"quoted\"");
        check(parse(serialize_to_string(json_escaped)) == json_escaped);
    }

    void test_parser_serializer_empty_structures() {
        TJsonValue json_empty_arr((TArray()));
        check(parse(serialize_to_string(json_empty_arr)) == json_empty_arr);

        TJsonValue json_empty_obj((TObject()));
        check(parse(serialize_to_string(json_empty_obj)) == json_empty_obj);
    }

    void test_parser_serializer_mixed_array() {
        TArray arr;
        arr.push_back(TJsonValue("string"));
        arr.push_back(TJsonValue(100));
        arr.push_back(TJsonValue(false));
        arr.push_back(TJsonValue()); // null
        
        TJsonValue json_doc(arr);
        auto val = parse(serialize_to_string(json_doc));
        check(val == json_doc);
    }

    void test_parser_serializer_deep_nesting() {
        TObject level_3;
        level_3["target"] = TJsonValue("found me");

        TObject level_2;
        level_2["next"] = TJsonValue(level_3);

        TObject level_1;
        level_1["nested_obj"] = TJsonValue(level_2);
        
        TArray arr;
        arr.push_back(TJsonValue(1));
        level_1["numbers"] = TJsonValue(arr);

        TJsonValue json_doc(level_1);

        auto val = parse(serialize_to_string(json_doc));
        check(val == json_doc);
    }


    void test_ostream_operator_simple() {
        TJsonValue json_int(42);
        std::ostringstream oss;
        
        oss << json_int;
        
        check(oss.str() == "42");
    }

    void test_ostream_operator_complex() {
        TObject obj;
        obj["key"] = TJsonValue("value");
        TJsonValue json_obj(obj);

        std::ostringstream oss;
        oss << json_obj;
        
        check(oss.str() == "{\"key\":\"value\"}");
    }

    void test_ostream_operator_chaining() {
        TJsonValue json_true(true);
        std::ostringstream oss;
        
        oss << "JSON result is: " << json_true << " (end)";
        
        check(oss.str() == "JSON result is: true (end)");
    }

    void test_ostream_operator_template_resolution() {
        TJsonValue json_null;
        std::stringstream ss; 
        
        ss << json_null;
        
        check(ss.str() == "null");
    }

    void test_pretty_mode_primitives() {
        check(serialize_to_string_pretty(TJsonValue()) == "null");
        check(serialize_to_string_pretty(TJsonValue(42)) == "42");
        check(serialize_to_string_pretty(TJsonValue(true)) == "true");
        check(serialize_to_string_pretty(TJsonValue("test")) == "\"test\"");
    }

    void test_pretty_mode_array() {
        TArray arr;
        arr.push_back(TJsonValue(1));
        arr.push_back(TJsonValue(2));
        arr.push_back(TJsonValue(3));
        
        check(serialize_to_string_pretty(TJsonValue(arr)) == "[1, 2, 3]");
    }

    void test_pretty_mode_empty_object() {
        check(serialize_to_string_pretty(TJsonValue(TObject())) == "{}");
    }

    void test_pretty_mode_object() {
        TObject obj;
        obj["key"] = TJsonValue(42);
        
        std::string expected = 
            "{\n"
            "    \"key\" : 42\n"
            "}";
            
        check(serialize_to_string_pretty(TJsonValue(obj)) == expected);
    }

    void test_pretty_mode_nested_object() {
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
            
        check(serialize_to_string_pretty(TJsonValue(outer_obj)) == expected);
        check(parse(expected) == TJsonValue(outer_obj));
    }

    void test_pretty_mode_object_with_array() {
        TArray arr;
        arr.push_back(TJsonValue(1));
        arr.push_back(TJsonValue(2));

        TObject obj;
        obj["arr"] = TJsonValue(arr);

        std::string expected = 
            "{\n"
            "    \"arr\" : [1, 2]\n"
            "}";
            
        check(serialize_to_string_pretty(TJsonValue(obj)) == expected);
        check(parse(expected) == TJsonValue(obj));
    }

    void test_parser_serializer_null_pretty() {
        TJsonValue json_null; 
        auto val = parse(serialize_to_string_pretty(json_null));
        check(val == json_null);
    }

    void test_parser_serializer_boolean_pretty() {
        TJsonValue json_true(true);
        check(parse(serialize_to_string_pretty(json_true)) == json_true);

        TJsonValue json_false(false);
        check(parse(serialize_to_string_pretty(json_false)) == json_false);
    }

    void test_parser_serializer_numbers_pretty() {
        TJsonValue json_int(42);
        check(parse(serialize_to_string_pretty(json_int)) == json_int);

        TJsonValue json_negative(-999);
        check(parse(serialize_to_string_pretty(json_negative)) == json_negative);

        TJsonValue json_double(3.5); 
        check(parse(serialize_to_string_pretty(json_double)) == json_double);
    }

    void test_parser_serializer_strings_pretty() {
        TJsonValue json_str("hello world");
        check(parse(serialize_to_string_pretty(json_str)) == json_str);

        TJsonValue json_empty_str("");
        check(parse(serialize_to_string_pretty(json_empty_str)) == json_empty_str);

        TJsonValue json_escaped("line1\nline2\t\"quoted\"");
        check(parse(serialize_to_string_pretty(json_escaped)) == "line1\nline2\t\"quoted\"");
    }

    void test_parser_serializer_empty_structures_pretty() {
        TJsonValue json_empty_arr((TArray())); 
        check(parse(serialize_to_string_pretty(json_empty_arr)) == json_empty_arr);

        TJsonValue json_empty_obj((TObject()));
        check(parse(serialize_to_string_pretty(json_empty_obj)) == json_empty_obj);
    }

    void test_parser_serializer_mixed_array_pretty() {
        TArray arr;
        arr.push_back(TJsonValue("string"));
        arr.push_back(TJsonValue(100));
        arr.push_back(TJsonValue(false));
        arr.push_back(TJsonValue()); 
        
        TJsonValue json_doc(arr);
        auto val = parse(serialize_to_string_pretty(json_doc));
        check(val == json_doc);
    }

    void test_parser_serializer_deep_nesting_pretty() {
        TObject level_3;
        level_3["target"] = TJsonValue("found me");

        TObject level_2;
        level_2["next"] = TJsonValue(level_3);

        TObject level_1;
        level_1["nested_obj"] = TJsonValue(level_2);
        
        TArray arr;
        arr.push_back(TJsonValue(1));
        level_1["numbers"] = TJsonValue(arr);

        TJsonValue json_doc(level_1);

        auto val = parse(serialize_to_string_pretty(json_doc));
        check(val == json_doc);
    }

    void test_json_string_serialization() {
        std::vector<std::string> test_cases = {
            "simple plain text",
            "", 
            "12345", 
            "He said: \"Hello!\"",
            "\"Start and end with quotes\"",
            "C:\\Users\\Default\\Desktop",
            "\\\\server\\share\\folder",
            "First line\nSecond line\tWith indent",
            "Ends with newline\n",
            "\"\\\b\f\n\r\t/", 
            "{\"key\": \"value\", \"array\": [1, 2, 3]}"
        };

        for (const std::string& original_str : test_cases) {
            TJsonValue original_val(original_str);
            
            std::string serialized_text = serialize_to_string(original_val);
            
            TJsonValue parsed_val = parse(serialized_text);
            
            check(parsed_val == original_val);
        }
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
        
    test_pretty_mode_primitives();
    test_pretty_mode_array();
    test_pretty_mode_empty_object();
    test_pretty_mode_object();
    test_pretty_mode_nested_object();
    test_pretty_mode_object_with_array();

    test_parser_serializer_null_pretty();
    test_parser_serializer_boolean_pretty();
    test_parser_serializer_numbers_pretty();
    test_parser_serializer_strings_pretty();
    test_parser_serializer_empty_structures_pretty();
    test_parser_serializer_mixed_array_pretty();
    test_parser_serializer_deep_nesting_pretty();

    test_json_string_serialization();
        
    std::cout << "All JSON serializer tests passed successfully!\n";
}
