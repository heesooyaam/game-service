#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/json/error.h>
#include <library/json/json_value.h>

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace NJson::NTests {

    TEST_CASE(test_concepts) {
        // 1. Тесты для CCharacter
        STATIC_CHECK(CCharacter<char>);
        STATIC_CHECK(CCharacter<wchar_t>);
        STATIC_CHECK(CCharacter<char32_t>);
        STATIC_CHECK(CCharacter<const char&>);
        STATIC_CHECK(CCharacter<unsigned char&&>);
        STATIC_CHECK(!CCharacter<int>);
        STATIC_CHECK(!CCharacter<double>);

        // 2. Тесты для CJsonInteger
        STATIC_CHECK(CJsonInteger<int>);
        STATIC_CHECK(CJsonInteger<long long>);
        STATIC_CHECK(CJsonInteger<std::uint64_t>);
        STATIC_CHECK(CJsonInteger<short>);
        STATIC_CHECK(CJsonInteger<const int&>);
        STATIC_CHECK(!CJsonInteger<bool>);
        STATIC_CHECK(!CJsonInteger<char>);
        STATIC_CHECK(!CJsonInteger<double>);

        // 3. Тесты для CJsonFloatingPoint
        STATIC_CHECK(CJsonFloatingPoint<float>);
        STATIC_CHECK(CJsonFloatingPoint<double>);
        STATIC_CHECK(CJsonFloatingPoint<long double>);
        STATIC_CHECK(CJsonFloatingPoint<const double&>);
        STATIC_CHECK(!CJsonFloatingPoint<int>);
        STATIC_CHECK(!CJsonFloatingPoint<bool>);

        // 4. Тесты для CJsonNumber
        STATIC_CHECK(CJsonNumber<int>);
        STATIC_CHECK(CJsonNumber<double>);
        STATIC_CHECK(CJsonNumber<float>);
        STATIC_CHECK(CJsonNumber<size_t>);
        STATIC_CHECK(CJsonNumber<const volatile long&&>);
        STATIC_CHECK(!CJsonNumber<bool>);
        STATIC_CHECK(!CJsonNumber<char>);
        STATIC_CHECK(!CJsonNumber<std::nullptr_t>);
        STATIC_CHECK(!CJsonNumber<void*>);
    }

    TEST_CASE(test_constructors) {
        // Null
        TJsonValue null_val;
        TJsonValue null_val2(TNull{});
        CHECK(null_val.is_null());
        CHECK(null_val2.is_null());
        CHECK(null_val.get_value_type() == EJsonType::Null);

        // Integer
        TJsonValue int_val = 42;
        CHECK(int_val.is_integer());
        CHECK(int_val.get_integer() == 42);
        CHECK(int_val.get_value_type() == EJsonType::Integer);

        // Double
        TJsonValue double_val = 3.14;
        CHECK(double_val.is_double());
        CHECK(double_val.get_double() == 3.14);

        // Boolean
        TJsonValue bool_val(true);
        CHECK(bool_val.is_boolean());
        CHECK(bool_val.get_boolean() == true);

        // String
        TJsonValue str_val1("c_string");
        TJsonValue str_val2(TString("std_string"));
        CHECK(str_val1.is_string() && str_val1.get_string() == "c_string");
        CHECK(str_val2.is_string() && str_val2.get_string() == "std_string");

        // Array
        TArray arr;
        arr.emplace_back(1);
        arr.emplace_back(2);
        TJsonValue arr_val(std::move(arr));
        CHECK(arr_val.is_array());
        CHECK(arr_val.size() == 2);
        CHECK(arr_val.get_array()[0].get_integer() == 1);

        // Object
        TObject obj;
        obj["key"] = 100;
        TJsonValue obj_val(std::move(obj));
        CHECK(obj_val.is_object());
        CHECK(obj_val.get_object().at("key").get_integer() == 100);
    }


    TEST_CASE(test_assignments) {
        TJsonValue val;

        val = 100;
        CHECK(val.is_integer() && val.get_integer() == 100);

        val = 3.1415;
        CHECK(val.is_double() && val.get_double() == 3.1415);

        val = true;
        CHECK(val.is_boolean() && val.get_boolean() == true);

        val = "reassigned";
        CHECK(val.is_string() && val.get_string() == "reassigned");

        val = TArray{TJsonValue(1), TJsonValue(2)};
        CHECK(val.is_array() && val.size() == 2);

        val = TObject{{"test", TJsonValue("value")}};
        CHECK(val.is_object() && val.get_object()["test"].get_string() == "value");

        val = TNull{};
        CHECK(val.is_null());
    }

    TEST_CASE(test_copy_semantics) {
        TJsonValue original = TArray{TJsonValue(1), TJsonValue(2)};

        // Copy constructor
        TJsonValue copied_construct(original);
        CHECK(copied_construct.is_array());
        CHECK(copied_construct == original);

        // Copy assignment
        TJsonValue copied_assign;
        copied_assign = original;
        CHECK(copied_assign == original);

        // Deep copy verification
        copied_assign.get_array()[0] = 99;
        CHECK(copied_assign != original);
        CHECK(original.get_array()[0].get_integer() == 1);

        // Self-assignment copy
        const auto* copied_assign_self = &copied_assign;
        copied_assign = *copied_assign_self;
        CHECK(copied_assign.is_array() && copied_assign.get_array()[0].get_integer() == 99);
    }

    TEST_CASE(test_move_semantics) {
        TJsonValue original = TString("movable_string");

        // Move constructor
        TJsonValue moved_construct(std::move(original));
        CHECK(moved_construct.is_string() && moved_construct.get_string() == "movable_string");
        CHECK(original.is_null());

        // Move assignment
        TJsonValue original_obj = TObject{{"key", 42}};
        TJsonValue moved_assign;
        moved_assign = std::move(original_obj);

        CHECK(moved_assign.is_object() && moved_assign.get_object()["key"].get_integer() == 42);
        CHECK(original_obj.is_null());

        // Self-assignment move
        auto* moved_assign_self = &moved_assign;
        moved_assign = std::move(*moved_assign_self);
        CHECK(moved_assign.is_object());
    }

    TEST_CASE(test_equality) {
        CHECK(TJsonValue(42) == TJsonValue(42));
        CHECK(TJsonValue(42) != TJsonValue(43));
        CHECK(TJsonValue(42) != TJsonValue(42.0)); // Strict type checking

        CHECK(TJsonValue("test") == TJsonValue("test"));
        CHECK(TJsonValue("test") != TJsonValue("test2"));

        TJsonValue arr1 = TArray{1, 2, 3};
        TJsonValue arr2 = TArray{1, 2, 3};
        TJsonValue arr3 = TArray{1, 2, 4};
        CHECK(arr1 == arr2);
        CHECK(arr1 != arr3);

        TJsonValue obj1 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj2 = TObject{{"a", 1}, {"b", true}};
        TJsonValue obj3 = TObject{{"a", 1}, {"b", false}};
        CHECK(obj1 == obj2);
        CHECK(obj1 != obj3);
    }

    TEST_CASE(test_deep_nesting) {
        TObject inner_obj;
        inner_obj["key"] = "value";

        TArray inner_arr;
        inner_arr.emplace_back(std::move(inner_obj));
        inner_arr.emplace_back(42);

        TObject root_obj;
        root_obj["data"] = std::move(inner_arr);

        TJsonValue root_json(std::move(root_obj));
        TJsonValue root_copy(root_json);

        CHECK(root_copy == root_json);

        root_copy.get_object()["data"].get_array()[0].get_object()["key"] = "NEW_VALUE";

        CHECK(root_copy != root_json);
        CHECK(root_json.get_object()["data"].get_array()[0].get_object()["key"].get_string() == "value");
    }

    TEST_CASE(test_getters_and_exceptions) {
        TJsonValue i = 42;
        TJsonValue s = "string";

        CHECK(i.get_integer() == 42);
        CHECK(s.get_string() == "string");

        bool thrown = false;
        try { i.get_string(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { s.get_array(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { i.get_object(); } catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_const_correctness) {
        const TJsonValue const_val = TArray{1, 2, 3};

        CHECK(const_val.is_array());
        CHECK(const_val.size() == 3);
        CHECK(const_val.get_array()[0].get_integer() == 1);
        CHECK(const_val.get_array()[1].get_integer() == 2);
    }

    TEST_CASE(test_unique_ownership_copy) {
        TJsonValue original_arr = TArray{TJsonValue(1), TJsonValue("test")};
        TJsonValue copied_arr = original_arr;

        const auto* orig_ptr = &original_arr.get_array()[0];
        const auto* copy_ptr = &copied_arr.get_array()[0];
        CHECK(orig_ptr != copy_ptr);

        const std::string& orig_str = original_arr.get_array()[1].get_string();
        const std::string& copy_str = copied_arr.get_array()[1].get_string();
        CHECK(&orig_str != &copy_str);
    }

    TEST_CASE(test_unique_ownership_move) {
        TJsonValue original_arr = TArray{TJsonValue(10), TJsonValue(20)};
        const auto* memory_address_before = &original_arr.get_array()[0];

        TJsonValue moved_arr = std::move(original_arr);
        const auto* memory_address_after = &moved_arr.get_array()[0];

        CHECK(memory_address_before == memory_address_after);
        CHECK(original_arr.is_null());
    }

    TEST_CASE(test_implicit_numeric_conversions) {
        short s = 10;
        unsigned int ui = 20;
        size_t st = 30;
        long long ll = 40;

        TJsonValue val1 = s;
        TJsonValue val2 = ui;
        TJsonValue val3 = st;
        TJsonValue val4 = ll;

        CHECK(val1.get_integer() == 10);
        CHECK(val2.get_integer() == 20);
        CHECK(val3.get_integer() == 30);
        CHECK(val4.get_integer() == 40);

        float f = 3.14f;
        TJsonValue val5 = f;
        CHECK(val5.is_double());
    }

    TEST_CASE(test_math_operations) {
        TJsonValue i = 10;
        i += 5;   CHECK(i.get_integer() == 15);
        i -= 2;   CHECK(i.get_integer() == 13);
        i *= 2;   CHECK(i.get_integer() == 26);
        i /= 2;   CHECK(i.get_integer() == 13);

        TJsonValue d = 2.5;
        d += 1.5; CHECK(d.get_double() == 4.0);
        d -= 1.0; CHECK(d.get_double() == 3.0);
        d *= 3.0; CHECK(d.get_double() == 9.0);
        d /= 2.0; CHECK(d.get_double() == 4.5);

        TJsonValue s = "hello";
        s += TString(" world");
        CHECK(s.get_string() == "hello world");

        bool thrown = false;
        try { s += 5; } catch (const NError::TJsonOperationError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_array_access) {
        TJsonValue arr = TArray{TJsonValue(10), TJsonValue(20), TJsonValue(30)};

        CHECK(arr[0].get_integer() == 10);
        arr[1] = 99;
        CHECK(arr.at(1).get_integer() == 99);

        const TJsonValue const_arr = arr;
        CHECK(const_arr[2].get_integer() == 30);
        CHECK(const_arr.at(2).get_integer() == 30);

        bool thrown = false;
        try { arr.at(100); } catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        CHECK(thrown);

        TJsonValue num = 42;
        thrown = false;
        try { num[0]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_object_access) {
        TJsonValue obj = TObject{{"name", TJsonValue("Alice")}, {"age", TJsonValue(25)}};

        CHECK(obj.contains("name"));
        CHECK(!obj.contains("height"));

        CHECK(obj["name"].get_string() == "Alice");
        CHECK(obj.at("age").get_integer() == 25);

        obj["height"] = 170.5;
        CHECK(obj.at("height").get_double() == 170.5);

        const TJsonValue const_obj = obj;
        CHECK(const_obj.at("name").get_string() == "Alice");

        bool thrown = false;
        try { const_obj.at("non_existent_key"); } catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        CHECK(thrown);

        TJsonValue str = "string";
        thrown = false;
        try { str["key"]; } catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_path_access) {
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

        CHECK(json.get_value_by_path("") == json);
        const TJsonValue& const_json = json;
        CHECK(const_json.get_value_by_path("") == json);

        CHECK(std::addressof(json.get_and_create_value_by_path("")) == std::addressof(json));

        CHECK(json.get_value_by_path("/users/0/id").get_integer() == 1);
        CHECK(json.get_value_by_path("/users/1/name").get_string() == "Bob");
        CHECK(json.get_value_by_path("/metadata/count").get_integer() == 2);

        CHECK(const_json.get_value_by_path("/users/0/name").get_string() == "Alice");

        TJsonValue dict = TObject{{"0", "zero_key_value"}};
        CHECK(dict.get_value_by_path("/0").get_string() == "zero_key_value");

        bool thrown = false;
        try { json.get_value_by_path("users/0"); }
        catch (const NError::TJsonBadPath&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { json.get_value_by_path("/users/5"); }
        catch (const NError::TJsonArrayOutOfRange&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { json.get_value_by_path("/metadata/version"); }
        catch (const NError::TJsonObjectOutOfRange&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { json.get_value_by_path("/metadata/count/value"); }
        catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_get_and_create_value_by_path) {
        TJsonValue json = TNull{};

        CHECK(json.get_and_create_value_by_path("") == json);
        CHECK(std::addressof(json.get_and_create_value_by_path("")) == std::addressof(json));

        json.get_and_create_value_by_path("/config/server/port") = 8080;
        CHECK(json.is_object());
        CHECK(json.get_value_by_path("/config/server/port").get_integer() == 8080);

        json.get_and_create_value_by_path("/config/server/host") = "localhost";
        CHECK(json.get_value_by_path("/config/server/host").get_string() == "localhost");

        json.get_and_create_value_by_path("/items/0") = "first";
        CHECK(json.get_value_by_path("/items").is_object());
        CHECK(json.get_value_by_path("/items/0").get_string() == "first");

        json.get_and_create_value_by_path("/real_array") = TArray{TJsonValue(10), TJsonValue(20)};
        json.get_and_create_value_by_path("/real_array/1") = 99;
        CHECK(json.get_value_by_path("/real_array/1").get_integer() == 99);

        bool thrown = false;
        try { json.get_and_create_value_by_path("/config/server/port/value"); }
        catch (const NError::TJsonTypeError&) { thrown = true; }
        CHECK(thrown);
    }

    TEST_CASE(test_exception_text) {
        bool thrown = false;
        try {
            TJsonValue val = 42;
            val.get_string();
        } catch (const NError::TJsonTypeError& e) {
            thrown = true;
            std::string msg = e.what();
            CHECK(msg.find("STRING") != std::string::npos);
            CHECK(msg.find("INTEGER") != std::string::npos);
        }
        CHECK(thrown);

        thrown = false;
        try {
            TJsonValue val = TArray{1, 2};
            val.at(5);
        } catch (const NError::TJsonArrayOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            CHECK(msg.find("array size = 2") != std::string::npos);
            CHECK(msg.find("index = 5") != std::string::npos);
        }
        CHECK(thrown);

        thrown = false;
        try {
            TJsonValue val = TObject{{"key", 1}};
            val.at("missing_key");
        } catch (const NError::TJsonObjectOutOfRange& e) {
            thrown = true;
            std::string msg = e.what();
            CHECK(msg.find("[JSON OBJECT OUT OF RANGE]: missing_key") != std::string::npos);
        }
        CHECK(thrown);
    }

    TEST_CASE(test_numeric_edge_cases) {
        bool thrown = false;

        // 1. Integer overflow test
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<std::uint64_t>::max();
        } catch (const NError::TJsonIntegerOutOfRange&) {
            thrown = true;
        }
        CHECK(thrown);

        TJsonValue valid_int = std::numeric_limits<int64_t>::max();
        CHECK(valid_int.get_integer() == std::numeric_limits<int64_t>::max());

        // 2. NaN
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<double>::quiet_NaN();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        CHECK(thrown);

        // 3. Infinity
        thrown = false;
        try {
            TJsonValue val = std::numeric_limits<double>::infinity();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        CHECK(thrown);

        // 4. -Infinity
        thrown = false;
        try {
            TJsonValue val = -std::numeric_limits<double>::infinity();
        } catch (const NError::TJsonBadDoubleNumber&) {
            thrown = true;
        }
        CHECK(thrown);

        TJsonValue valid_double = std::numeric_limits<double>::max();
        CHECK(valid_double.get_double() == std::numeric_limits<double>::max());

        TJsonValue assign_val;

        thrown = false;
        try { assign_val = std::numeric_limits<std::uint64_t>::max(); }
        catch (const NError::TJsonIntegerOutOfRange&) { thrown = true; }
        CHECK(thrown);

        thrown = false;
        try { assign_val = std::numeric_limits<double>::infinity(); }
        catch (const NError::TJsonBadDoubleNumber&) { thrown = true; }
        CHECK(thrown);
    }

} // namespace NJson::NTests
