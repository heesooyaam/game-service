#pragma once

#include <source_location>
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

    std::vector<TTestCase>& test_cases();

    class TTestRegistration {
    public:
        TTestRegistration(std::string_view name, TTestFunction function);
    };

    [[noreturn]] void fail(
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    );

    void check(
        bool result,
        std::string_view expression,
        const std::source_location& location = std::source_location::current()
    );

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

    int run_all_tests();

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
