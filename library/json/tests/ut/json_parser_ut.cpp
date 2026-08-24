#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/json/error.h>
#include <library/json/json_parser.h>
#include <library/json/json_value.h>
#include <library/common/parse_number.h>

#include <cctype>
#include <cstdlib>
#include <string_view>

namespace {

    void check_throws(std::string_view bad_json) {
        CHECK_THROWS_AS(
            NJson::TJsonParser(bad_json).parse(),
            NJson::NError::TJsonParserError
        );
    }
}

namespace NJson::NTests {

     TEST_CASE(test_parser_primitives) {
        CHECK(TJsonParser("null").parse().is_null());

        CHECK(TJsonParser("true").parse().get_boolean() == true);
        CHECK(TJsonParser("false").parse().get_boolean() == false);

        auto int_val = TJsonParser("42").parse();
        CHECK(int_val.is_integer());
        CHECK(int_val.get_integer() == 42);

        auto double_val = TJsonParser("-3.14").parse();
        CHECK(double_val.is_double());
        CHECK(double_val.get_double() == -3.14);

        auto str_val = TJsonParser(R"("hello world")").parse();
        CHECK(str_val.is_string());
        CHECK(str_val.get_string() == "hello world");
    }

    TEST_CASE(test_parser_string_escaping) {
        auto s1 = TJsonParser(R"("Say \"Hello\"")").parse();
        CHECK(s1.get_string() == "Say \"Hello\"");

        auto s2 = TJsonParser(R"("C:\\Program Files\\MyGame")").parse();
        CHECK(s2.get_string() == "C:\\Program Files\\MyGame");

        auto s3 = TJsonParser(R"("Line1\nLine2\tTabbed")").parse();
        CHECK(s3.get_string() == "Line1\nLine2\tTabbed");

        auto s4 = TJsonParser(R"("pppp \" ppp")").parse();
        CHECK(s4.get_string() == "pppp \" ppp");

        auto s5 = TJsonParser(R"("\b\f\n\r\t\/\\\"")").parse();
        CHECK(s5.get_string() == "\b\f\n\r\t/\\\"");
    }

    TEST_CASE(test_parser_arrays_and_objects) {
        auto arr = TJsonParser(R"([1, "two", false])").parse();
        CHECK(arr.is_array());
        CHECK(arr.size() == 3);
        CHECK(arr[0].get_integer() == 1);
        CHECK(arr[1].get_string() == "two");
        CHECK(arr[2].get_boolean() == false);

        auto obj = TJsonParser(R"({"key": 42, "empty": {}})").parse();
        CHECK(obj.is_object());
        CHECK(obj.contains("key"));
        CHECK(obj["key"].get_integer() == 42);

        CHECK(obj.contains("empty"));
        CHECK(obj["empty"].is_object());
    }

    TEST_CASE(test_parser_whitespaces) {
        std::string_view messy_json = R"(
            {
                "array" : [
                    1 ,
                    2

                    ]
            }
        )";

        auto result = TJsonParser(messy_json).parse();

        CHECK(result.is_object());
        CHECK(result.contains("array"));
        CHECK(result["array"].is_array());
        CHECK(result["array"][0].get_integer() == 1);
        CHECK(result["array"][1].get_integer() == 2);

        CHECK(TJsonParser("   \t\n true \r  ").parse().get_boolean() == true);
    }

    TEST_CASE(test_parser_number_edge_cases) {
        CHECK(TJsonParser("0").parse().get_integer() == 0);
        CHECK(TJsonParser("-42").parse().get_integer() == -42);

        CHECK(TJsonParser("-0.0").parse().get_double() == 0.0);

        auto exp_val = TJsonParser("1.23e3").parse();
        CHECK(exp_val.is_double());
    }

    TEST_CASE(test_parser_nested_structures) {
        CHECK(TJsonParser("[]").parse().size() == 0);
        CHECK(TJsonParser("{}").parse().is_object());

        auto deep_arr = TJsonParser("[[[[42]]]]").parse();
        CHECK(deep_arr[0][0][0][0].get_integer() == 42);

        auto deep_obj = TJsonParser(R"({"a": {"b": {"c": "d"}}})").parse();
        CHECK(deep_obj["a"]["b"]["c"].get_string() == "d");
    }

    TEST_CASE(test_parser_invalid_json) {
        check_throws(R"({"a": 1} 123)");
        check_throws("[1, 2] ]");

        check_throws("[1, 2,]");
        check_throws(R"({"a": 1,})");

        check_throws("[1, 2");
        check_throws(R"({"a": 1)");
        check_throws(R"("unclosed string)");

        check_throws("{a: 1}");
        check_throws(R"({"a" 1})");
        check_throws(R"({"a": })");

        check_throws("nulll");
        check_throws("tru");
        check_throws("flase");

        check_throws(R"("pp \ p;pp")");
        check_throws(R"("bad escape \z")");
        check_throws(R"("trailing slash \")");
    }

    TEST_CASE(test_parser_hardcore_nesting) {
        auto deep_arr = TJsonParser("[[[[[[[[[[42]]]]]]]]]]").parse();
        CHECK(deep_arr[0][0][0][0][0][0][0][0][0][0].get_integer() == 42);

        std::string_view obj_str = R"({"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":{"x":777}}}}}}}}}})";
        auto deep_obj = TJsonParser(obj_str).parse();
        CHECK(deep_obj["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"]["x"].get_integer() == 777);

        auto mix = TJsonParser(R"([{"a": [{"b": [{"c": 99}]}]}])").parse();
        CHECK(mix[0]["a"][0]["b"][0]["c"].get_integer() == 99);
    }

    TEST_CASE(test_parser_empty_strings_and_documents) {
        auto empty_str = TJsonParser(R"("")").parse();
        CHECK(empty_str.is_string());
        CHECK(empty_str.get_string() == "");

        auto obj_with_empty = TJsonParser(R"({"": ""})").parse();
        CHECK(obj_with_empty.is_object());
        CHECK(obj_with_empty.contains(""));
        CHECK(obj_with_empty[""].get_string() == "");

        check_throws("");
        check_throws("   \t\n  ");

        check_throws(R"({"a": 1} 42)");
        check_throws(R"([1, 2] [3, 4])");
    }

    TEST_CASE(test_parser_complex_mutations) {
        TJsonValue val = TObject{};

        val.get_object()["dynamic_key"] = 42;
        CHECK(val["dynamic_key"].is_integer());

        val.get_object()["dynamic_key"] = TArray{1, 2, 3};
        CHECK(val["dynamic_key"].is_array());
        CHECK(val["dynamic_key"][1].get_integer() == 2);

        val.get_object()["dynamic_key"] = TObject{{"nested", "wow"}};
        CHECK(val["dynamic_key"].is_object());
        CHECK(val["dynamic_key"]["nested"].get_string() == "wow");

        val.get_object()["dynamic_key"] = TNull{};
        CHECK(val["dynamic_key"].is_null());
    }

    TEST_CASE(test_parser_more_invalid_edge_cases) {
        check_throws("{'single_quotes': 1}");
        check_throws("['string']");

        check_throws("{unquoted_key: 1}");
        check_throws(R"({"key": \n 1})");

        check_throws(R"([1, 2,, 3])");
        check_throws(R"({"a": 1,, "b": 2})");
    }

    TEST_CASE(test_parser_hard_check) {
        std::string str = std::string(1000, '[') + std::string("{\"123\" : 123}") + std::string(1000, ']');
        auto json = TJsonParser(str).parse();
        for (size_t i = 0; i < 1000; ++i) {
            json = json[0];
        }
        CHECK(json["123"] == 123);
    }

} //namespace NJson::NTests
