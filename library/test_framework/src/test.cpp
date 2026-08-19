#include <library/test_framework/test.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace NTesting {

    std::vector<TTestCase>& test_cases() {
        static std::vector<TTestCase> cases;
        return cases;
    }

    TTestRegistration::TTestRegistration(std::string_view name, TTestFunction function) {
        test_cases().push_back({name, function});
    }

    [[noreturn]] void fail(
        std::string_view message,
        const std::source_location& location
    ) {
        std::ostringstream output;
        output << location.file_name() << ':' << location.line() << ": " << message;
        throw std::runtime_error(output.str());
    }

    void check(
        bool result,
        std::string_view expression,
        const std::source_location& location
    ) {
        if (!result) {
            fail("CHECK(" + std::string(expression) + ") failed", location);
        }
    }

    int run_all_tests() {
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
