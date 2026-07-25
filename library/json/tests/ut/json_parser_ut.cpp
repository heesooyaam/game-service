#include <library/json/error.h>
#include <library/json/json_parser.h>
#include <library/json/json_value.h>
#include <library/common/parse_number.h>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
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

     void test_primitives() {
        check(TJsonParser("null").parse().is_null());

        check(TJsonParser("true").parse().get_boolean() == true);
        check(TJsonParser("false").parse().get_boolean() == false);

        auto int_val = TJsonParser("42").parse();
        check(int_val.is_integer());
        check(int_val.get_integer() == 42);

        auto double_val = TJsonParser("-3.14").parse();
        check(double_val.is_double());
        check(double_val.get_double() == -3.14);

        auto str_val = TJsonParser("\"hello world\"").parse();
        check(str_val.is_string());
        check(str_val.get_string() == "hello world");
    }

    void test_string_escaping() {
        auto s1 = TJsonParser(R"("Say \"Hello\"")").parse();
        check(s1.get_string() == "Say \"Hello\"");

        auto s2 = TJsonParser(R"("C:\\Program Files\\MyGame")").parse();
        check(s2.get_string() == "C:\\Program Files\\MyGame");

        auto s3 = TJsonParser(R"("Line1\nLine2\tTabbed")").parse();
        check(s3.get_string() == "Line1\nLine2\tTabbed");

        auto s4 = TJsonParser(R"("pppp \" ppp")").parse();
        check(s4.get_string() == "pppp \" ppp");

        auto s5 = TJsonParser(R"("\b\f\n\r\t\/\\\"")").parse();
        check(s5.get_string() == "\b\f\n\r\t/\\\"");
    }

    void test_arrays_and_objects() {
        auto arr = TJsonParser("[1, \"two\", false]").parse();
        check(arr.is_array());
        check(arr.size() == 3);
        check(arr[0].get_integer() == 1);
        check(arr[1].get_string() == "two");
        check(arr[2].get_boolean() == false);

        auto obj = TJsonParser("{\"key\": 42, \"empty\": {}}").parse();
        check(obj.is_object());
        check(obj.contains("key"));
        check(obj["key"].get_integer() == 42);
        
        check(obj.contains("empty"));
        check(obj["empty"].is_object());
    }

    void test_whitespaces() {
        std::string_view messy_json = R"(
            {
                "array" : [
                    1 , 
                    2               
                    
                    ]
            }
        )";

        auto result = TJsonParser(messy_json).parse();
        
        check(result.is_object());
        check(result.contains("array"));
        check(result["array"].is_array());
        check(result["array"][0].get_integer() == 1);
        check(result["array"][1].get_integer() == 2);

        check(TJsonParser("   \t\n true \r  ").parse().get_boolean() == true);
    }
    
    // Вспомогательная функция для проверки исключений
    void check_throws(std::string_view bad_json) {
        bool caught_error = false;
        try {
            TJsonParser(bad_json).parse();
        } catch (const NError::TJsonParserError&) {
            caught_error = true; 
        }
        
        if (!caught_error) {
            std::cerr << "TEST FAILED: Parser should have thrown on: " << bad_json << "\n";
        }
        check(caught_error);
    }

    void test_number_edge_cases() {
        check(TJsonParser("0").parse().get_integer() == 0);
        check(TJsonParser("-42").parse().get_integer() == -42);
        
        check(TJsonParser("-0.0").parse().get_double() == 0.0);
        
        auto exp_val = TJsonParser("1.23e3").parse();
        check(exp_val.is_double());
    }

    void test_nested_structures() {
        check(TJsonParser("[]").parse().size() == 0);
        check(TJsonParser("{}").parse().is_object());

        auto deep_arr = TJsonParser("[[[[42]]]]").parse();
        check(deep_arr[0][0][0][0].get_integer() == 42);

        auto deep_obj = TJsonParser("{\"a\": {\"b\": {\"c\": \"d\"}}}").parse();
        check(deep_obj["a"]["b"]["c"].get_string() == "d");
    }

    void test_invalid_json() {
        check_throws("{\"a\": 1} 123");
        check_throws("[1, 2] ]");

        check_throws("[1, 2,]");
        check_throws("{\"a\": 1,}");

        check_throws("[1, 2");
        check_throws("{\"a\": 1");
        check_throws("\"unclosed string");

        check_throws("{a: 1}");      
        check_throws("{\"a\" 1}");    
        check_throws("{\"a\": }");    

        check_throws("nulll");
        check_throws("tru");
        check_throws("flase");

        check_throws(R"("pp \ p;pp")"); 
        check_throws(R"("bad escape \z")"); 
        check_throws(R"("trailing slash \")"); 
    }    

    void test_hardcore_nesting() {
        auto deep_arr = TJsonParser("[[[[[[[[[[42]]]]]]]]]]").parse();
        check(deep_arr[0][0][0][0][0][0][0][0][0][0].get_integer() == 42);

        std::string_view obj_str = R"({"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":777}}}}}}}}}})";
        auto deep_obj = TJsonParser(obj_str).parse();
        check(deep_obj["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"].get_integer() == 777);

        auto mix = TJsonParser(R"([{"a": [{"b": [{"c": 99}]}]}])").parse();
        check(mix[0]["a"][0]["b"][0]["c"].get_integer() == 99);
    }

    void test_empty_strings_and_documents() {
        auto empty_str = TJsonParser(R"("")").parse();
        check(empty_str.is_string());
        check(empty_str.get_string() == "");

        auto obj_with_empty = TJsonParser(R"({"": ""})").parse();
        check(obj_with_empty.is_object());
        check(obj_with_empty.contains(""));
        check(obj_with_empty[""].get_string() == "");

        check_throws("");
        check_throws("   \t\n  "); 

        check_throws(R"({"a": 1} 42)");
        check_throws(R"([1, 2] [3, 4])");
    }

    void test_complex_mutations() {
        TJsonValue val = TObject{};
        
        val.get_object()["dynamic_key"] = 42;
        check(val["dynamic_key"].is_integer());
        
        val.get_object()["dynamic_key"] = TArray{1, 2, 3};
        check(val["dynamic_key"].is_array());
        check(val["dynamic_key"][1].get_integer() == 2);

        val.get_object()["dynamic_key"] = TObject{{"nested", "wow"}};
        check(val["dynamic_key"].is_object());
        check(val["dynamic_key"]["nested"].get_string() == "wow");
        
        val.get_object()["dynamic_key"] = TNull{};
        check(val["dynamic_key"].is_null());
    }

    void test_more_invalid_edge_cases() {
        check_throws("{'single_quotes': 1}"); 
        check_throws("['string']"); 

        check_throws("{unquoted_key: 1}");   
        check_throws(R"({"key": \n 1})");    

        check_throws(R"([1, 2,, 3])");       
        check_throws(R"({"a": 1,, "b": 2})");
    }

    void test_hard_check() {
        std::string str = std::string(1000, '[') + std::string("{\"123\" : 123}") + std::string(1000, ']');
        auto json = TJsonParser(str).parse();
        for (size_t i = 0; i < 1000; ++i) {
            json = json[0];
        }
        check(json["123"] == 123);
    }

} //namespace NJson::NTests


int main() {
    using namespace NJson::NTests;

    test_primitives();
    test_string_escaping(); 
    test_arrays_and_objects();
    test_whitespaces();

    test_number_edge_cases();
    test_nested_structures();
    
    test_invalid_json();
    test_empty_strings_and_documents();
    test_more_invalid_edge_cases();

    test_hardcore_nesting();
    test_complex_mutations();

    test_hard_check();

    std::cout << "All TJsonValue tests passed successfully! You are breathtaking!" << std::endl;
    return 0;
}
