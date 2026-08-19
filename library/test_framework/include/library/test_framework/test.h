#pragma once

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace NTesting {

    using TTestFunction = void (*)();

    struct TTestCase {
        std::string_view name;
        TTestFunction function;
    };

    inline std::vector<TTestCase>& test_cases() {
        static std::vector<TTestCase> cases;
        return cases;
    }

    class TTestRegistration {
    public:
        TTestRegistration(std::string_view name, TTestFunction function) {
            test_cases().push_back({name, function});
        }
    };

    [[noreturn]] inline void fail(
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    ) {
        std::ostringstream output;
        output << location.file_name() << ':' << location.line() << ": " << message;
        throw std::runtime_error(output.str());
    }

    inline void check(
        bool result,
        std::string_view expression,
        const std::source_location& location = std::source_location::current()
    ) {
        if (!result) {
            fail("CHECK(" + std::string(expression) + ") failed", location);
        }
    }

    template <class TException, class TFunction>
    void check_throws(
        TFunction&& function,
        std::string_view expression,
        std::string_view exception_name,
        const std::source_location& location = std::source_location::current()
    ) {
        try {
            std::forward<TFunction>(function)();
        } catch (const TException&) {
            return;
        } catch (...) {
            fail(
                std::string(expression) + " threw an unexpected exception instead of "
                    + std::string(exception_name),
                location
            );
        }

        fail(
            std::string(expression) + " did not throw " + std::string(exception_name),
            location
        );
    }

    inline int run_all_tests() {
        const auto& cases = test_cases();
        if (cases.empty()) {
            std::cerr << "No test cases registered\n";
            return EXIT_FAILURE;
        }

        std::size_t failed = 0;
        for (const auto& test_case : cases) {
            try {
                test_case.function();
                std::cout << "[ OK ] " << test_case.name << '\n';
            } catch (const std::exception& error) {
                ++failed;
                std::cerr << "[FAIL] " << test_case.name << '\n'
                          << "       " << error.what() << '\n';
            } catch (...) {
                ++failed;
                std::cerr << "[FAIL] " << test_case.name << '\n'
                          << "       unknown exception\n";
            }
        }

        std::cout << "\n" << cases.size() << " test case(s): "
                  << cases.size() - failed << " passed, " << failed << " failed\n";
        return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

} // namespace NTesting

#define NTEST_DETAIL_CONCAT_IMPL(left, right) left##right
#define NTEST_DETAIL_CONCAT(left, right) NTEST_DETAIL_CONCAT_IMPL(left, right)

#define NTEST_DETAIL_TEST_CASE(name, line)                                              \
    static void NTEST_DETAIL_CONCAT(ntest_case_, line)();                              \
    [[maybe_unused]] static const ::NTesting::TTestRegistration                        \
        NTEST_DETAIL_CONCAT(ntest_registration_, line){                                \
            #name, &NTEST_DETAIL_CONCAT(ntest_case_, line)};                           \
    static void NTEST_DETAIL_CONCAT(ntest_case_, line)()

#define TEST_CASE(name) NTEST_DETAIL_TEST_CASE(name, __LINE__)
#define CHECK(expression) ::NTesting::check(static_cast<bool>(expression), #expression)
#define CHECK_EQ(actual, expected) CHECK((actual) == (expected))
#define CHECK_NE(actual, expected) CHECK((actual) != (expected))
#define CHECK_THROWS_AS(expression, exception_type)                                    \
    ::NTesting::check_throws<exception_type>(                                           \
        [&] { static_cast<void>(expression); }, #expression, #exception_type)
#define STATIC_CHECK(expression) static_assert((expression), #expression)

#ifdef NTEST_MAIN
int main() {
    return ::NTesting::run_all_tests();
}
#endif
