#include <library/json/json_parser.h>
#include <library/json/json_serializer.h>
#include <library/json/json_value.h>

#include <sstream>
#include <string>
#include <vector>

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
        return oss.str();
    }

}

namespace NJson::NTests {

    void run_round_trip_tests(const std::vector<std::string>& cases) {
        for (const std::string& raw_json : cases) {
            TJsonValue original_val1 = parse(raw_json);
            TJsonValue original_val2 = parse(raw_json);
            std::string serialized_text1 = serialize_to_string(original_val1);
            std::string serialized_text2 = serialize_to_string(original_val2);
            TJsonValue parsed_val1 = parse(serialized_text1);
            TJsonValue parsed_val2 = parse(serialized_text2);
            check(parsed_val1 == original_val1);
            check(parsed_val2 == original_val2);
            check(parsed_val1 == parsed_val2);
            check(original_val1 == original_val2);
            check(serialized_text1 == serialized_text2);
        }
    }

    void test_null() {
        run_round_trip_tests({
            R"(null)"
        });
    }

    void test_booleans() {
        run_round_trip_tests({
            R"(true)",
            R"(false)"
        });
    }

    void test_integers() {
        run_round_trip_tests({
            R"(0)",
            R"(1)",
            R"(-1)",
            R"(42)",
            R"(-999)",
            R"(2147483647)",
            R"(-2147483648)",
            R"(9223372036854775807)",
            R"(-9223372036854775808)"
        });
    }

    void test_double1() {
        run_round_trip_tests({
            R"(0.0)",
            R"(-0.0)",
            R"(3.141592653589793)",
            R"(-0.0000001)",
            R"(1.23e+4)",
            R"(-5.67E-8)",
            R"(1e10)",
            R"(1.0E-10)"
        });
    }

    void test_strings() {
        run_round_trip_tests({
            R"("")",
            R"("a")",
            R"("simple text")",
            R"("He said: \"Hello!\"")",
            R"("C:\\Windows\\System32")",
            R"("Line1\nLine2\tTabbed\r\b\f")",
            R"("Symbols: !@#$%^&*()_+{}|:<>?~`-=[]")",
            R"("\"\\\/\b\f\n\r\t")",
            R"(" ")",
            R"("12345")"
        });
    }

    void test_arrays() {
        run_round_trip_tests({
            R"([])",
            R"([1, 2, 3])",
            R"(["a", "b", "c"])",
            R"([true, false, null])",
            R"([1, "text", true, null, 3.14])",
            R"([[], [1], [1, 2]])"
        });
    }

    void test_objects() {
        run_round_trip_tests({
            R"({})",
            R"({"a": 1})",
            R"({"key": "value", "number": 100})",
            R"({"": ""})",
            R"({" ": " "})",
            R"({"true": true, "false": false, "null": null})"
        });
    }

    void test_nested_structures() {
        run_round_trip_tests({
            R"({"users": [{"id": 1, "name": "A"}, {"id": 2, "name": "B"}], "active": false, "meta": {"page": 1, "total": 100}})",
            R"([{"a": [1, 2, 3]}, {"b": {"c": {}}}])",
            R"({"level1": {"level2": {"level3": {"level4": "deep"}}}})",
            R"([[[[[]]]]])"
        });
    }

    void test_double2() {
        run_round_trip_tests({
            R"(0.0)",
            R"(-0.0)",
            R"(3.141592653589793)",
            R"(-0.0000001)",
            
            R"(0e0)",
            R"(-0e0)",
            R"(0.0e-10)",
            R"(-0.0e+10)",

            R"(1e0)",          
            R"(-1E0)",        
            R"(1.5e1)",       
            R"(1.5E+1)",        
            R"(1.5e-1)",      
            R"(12345e-3)",      

            R"(1.23456789012345)",
            R"(-9.876543210987654)",
            R"(1000000000000000.5)", 
            R"(0.0000000000000001)",

            R"(1.7976931348623157e+308)",
            R"(-1.7976931348623157e+308)",
            
            R"(2.2250738585072014e-308)",
            R"(-2.2250738585072014e-308)",

            R"(4.9406564584124654e-324)",
            R"(-4.9406564584124654e-324)",

            R"(1.000000000000001)",
            R"(0.9999999999999999)",

            R"(9007199254740991.0)", 
            R"(-9007199254740991.0)", 
            R"(9007199254740992.0)", 
            
            R"(1e+0005)",
            R"(2.5E-003)"
        });
    }

} //namespace NJson::NTests

int main() {
    using namespace NJson::NTests;

    test_null();
    test_booleans();
    test_integers();
    test_double1();
    test_double2();
    test_strings();
    test_arrays();
    test_objects();
    test_nested_structures();
    
    return 0;
}
