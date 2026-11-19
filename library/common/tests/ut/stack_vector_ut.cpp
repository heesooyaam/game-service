#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/common/struct/stack_vector/error.h>
#include <library/common/struct/stack_vector/stack_vector.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace NCommon::NStruct::NTests {

    namespace {

        struct TTracked {
            static inline int alive = 0;
            static inline int copies = 0;
            static inline int moves = 0;
            static inline int destroyed = 0;

            int value = 0;

            explicit TTracked(int initial_value = 0)
                : value(initial_value)
            {
                ++alive;
            }

            TTracked(const TTracked& other)
                : value(other.value)
            {
                ++alive;
                ++copies;
            }

            TTracked(TTracked&& other) noexcept
                : value(other.value)
            {
                ++alive;
                ++moves;
                other.value = -1;
            }

            TTracked& operator=(const TTracked&) = default;
            TTracked& operator=(TTracked&&) noexcept = default;

            ~TTracked() {
                --alive;
                ++destroyed;
            }

            static void reset() {
                alive = 0;
                copies = 0;
                moves = 0;
                destroyed = 0;
            }
        };

        struct TCopyBomb {
            static inline int alive = 0;
            static inline int copies = 0;
            static inline int throw_on_copy = -1;

            int value = 0;

            explicit TCopyBomb(int initial_value = 0)
                : value(initial_value)
            {
                ++alive;
            }

            TCopyBomb(const TCopyBomb& other)
                : value(other.value)
            {
                ++copies;

                if (copies == throw_on_copy) {
                    throw std::runtime_error("copy failed");
                }

                ++alive;
            }

            TCopyBomb(TCopyBomb&& other) noexcept
                : value(other.value)
            {
                ++alive;
                other.value = -1;
            }

            TCopyBomb& operator=(const TCopyBomb&) = default;
            TCopyBomb& operator=(TCopyBomb&&) noexcept = default;

            ~TCopyBomb() {
                --alive;
            }

            static void reset() {
                alive = 0;
                copies = 0;
                throw_on_copy = -1;
            }
        };

        struct alignas(64) TAlignedValue {
            int value = 0;

            explicit TAlignedValue(int initial_value = 0)
                : value(initial_value)
            {}
        };

    } // namespace

    TEST_CASE(test_default_constructor) {
        TStackVector<int, 4> vector;

        CHECK(vector.empty());
        CHECK_EQ(vector.size(), 0);
        CHECK_EQ(vector.capacity(), 4);

        STATIC_CHECK((TStackVector<int, 4>::capacity() == 4));
    }

    TEST_CASE(test_initializer_list_constructor) {
        TStackVector<int, 4> vector{1, 2, 3};

        CHECK_EQ(vector.size(), 3);
        CHECK_EQ(vector[0], 1);
        CHECK_EQ(vector[1], 2);
        CHECK_EQ(vector[2], 3);
        CHECK_EQ(vector.front(), 1);
        CHECK_EQ(vector.back(), 3);
    }

    TEST_CASE(test_initializer_list_capacity_exceeded) {
        using TVector = TStackVector<int, 2>;

        CHECK_THROWS_AS(
            (TVector{1, 2, 3}),
            NError::TStackVectorCapacityExceeded
        );
    }

    TEST_CASE(test_push_back_and_data) {
        TStackVector<int, 4> vector;

        int value = 10;
        vector.push_back(value);
        vector.push_back(20);

        CHECK_EQ(vector.size(), 2);
        CHECK_EQ(vector[0], 10);
        CHECK_EQ(vector[1], 20);

        CHECK(vector.data() == &vector[0]);
        CHECK_EQ(vector.data()[1], 20);
        CHECK_EQ(&vector[1] - &vector[0], 1);

        TStackVector<int, 4> vector2;
        vector2.unchecked_push_back(10); 
        vector2.unchecked_push_back(20);

        CHECK_EQ(vector2.size(), 2);
        CHECK_EQ(vector2[0], 10);
        CHECK_EQ(vector2[1], 20);

        CHECK(vector2.data() == &vector2[0]);
        CHECK_EQ(vector2.data()[1], 20);
        CHECK_EQ(&vector2[1] - &vector2[0], 1);
    }

    TEST_CASE(test_emplace_back) {
        TStackVector<std::string, 3> vector;

        std::string& first = vector.emplace_back("hello");
        std::string& second = vector.emplace_back(5, 'x');

        CHECK_EQ(vector.size(), 2);
        CHECK(first == "hello");
        CHECK(second == "xxxxx");
        CHECK(&first == &vector[0]);
        CHECK(&second == &vector[1]);
    }

    TEST_CASE(test_capacity_exceeded_on_push_back) {
        TStackVector<int, 2> vector;

        vector.push_back(1);
        vector.push_back(2);

        CHECK_THROWS_AS(
            vector.push_back(3),
            NError::TStackVectorCapacityExceeded
        );

        CHECK_EQ(vector.size(), 2);
        CHECK_EQ(vector[0], 1);
        CHECK_EQ(vector[1], 2);
    }

    TEST_CASE(test_at) {
        TStackVector<int, 3> vector{10, 20, 30};

        CHECK_EQ(vector.at(0), 10);
        CHECK_EQ(vector.at(2), 30);

        vector.at(1) = 50;
        CHECK_EQ(vector[1], 50);

        CHECK_THROWS_AS(vector.at(3), std::out_of_range);
        CHECK_THROWS_AS(vector.at(100), std::out_of_range);
    }

    TEST_CASE(test_front_and_back) {
        TStackVector<int, 4> vector;

        vector.push_back(10);

        CHECK_EQ(vector.front(), 10);
        CHECK_EQ(vector.back(), 10);

        vector.push_back(20);
        vector.push_back(30);

        CHECK_EQ(vector.front(), 10);
        CHECK_EQ(vector.back(), 30);

        vector.front() = 1;
        vector.back() = 3;

        CHECK_EQ(vector[0], 1);
        CHECK_EQ(vector[2], 3);
    }

    TEST_CASE(test_pop_back_and_clear) {
        TTracked::reset();

        {
            TStackVector<TTracked, 4> vector;

            vector.emplace_back(1);
            vector.emplace_back(2);
            vector.emplace_back(3);

            CHECK_EQ(TTracked::alive, 3);

            vector.pop_back();

            CHECK_EQ(vector.size(), 2);
            CHECK_EQ(TTracked::alive, 2);
            CHECK_EQ(TTracked::destroyed, 1);
            CHECK_EQ(vector.back().value, 2);

            vector.clear();

            CHECK(vector.empty());
            CHECK_EQ(TTracked::alive, 0);
            CHECK_EQ(TTracked::destroyed, 3);
        }

        CHECK_EQ(TTracked::alive, 0);
        CHECK_EQ(TTracked::destroyed, 3);
    }

    TEST_CASE(test_destructor_destroys_elements) {
        TTracked::reset();

        {
            TStackVector<TTracked, 4> vector;

            vector.emplace_back(1);
            vector.emplace_back(2);

            CHECK_EQ(TTracked::alive, 2);
        }

        CHECK_EQ(TTracked::alive, 0);
        CHECK_EQ(TTracked::destroyed, 2);
    }

    TEST_CASE(test_copy_constructor) {
        TStackVector<std::string, 4> source{"one", "two", "three"};

        TStackVector<std::string, 4> copy(source);

        CHECK_EQ(copy.size(), source.size());
        CHECK(copy[0] == "one");
        CHECK(copy[1] == "two");
        CHECK(copy[2] == "three");

        source[0] = "changed";

        CHECK(copy[0] == "one");
        CHECK(source[0] == "changed");
    }

    TEST_CASE(test_move_constructor) {
        TTracked::reset();

        {
            TStackVector<TTracked, 4> source;

            source.emplace_back(10);
            source.emplace_back(20);

            TStackVector<TTracked, 4> moved(std::move(source));

            CHECK(source.empty());
            CHECK_EQ(moved.size(), 2);
            CHECK_EQ(moved[0].value, 10);
            CHECK_EQ(moved[1].value, 20);
            CHECK_EQ(TTracked::moves, 2);
            CHECK_EQ(TTracked::alive, 2);
        }

        CHECK_EQ(TTracked::alive, 0);
    }

    TEST_CASE(test_copy_assignment) {
        TStackVector<std::string, 4> source{"one", "two", "three"};
        TStackVector<std::string, 4> destination{"old"};

        destination = source;

        CHECK_EQ(destination.size(), 3);
        CHECK(destination[0] == "one");
        CHECK(destination[1] == "two");
        CHECK(destination[2] == "three");

        source[1] = "changed";

        CHECK(destination[1] == "two");
    }

    TEST_CASE(test_move_assignment) {
        TStackVector<std::string, 4> source{"one", "two", "three"};
        TStackVector<std::string, 4> destination{"old", "values"};

        destination = std::move(source);

        CHECK(source.empty());
        CHECK_EQ(destination.size(), 3);
        CHECK(destination[0] == "one");
        CHECK(destination[1] == "two");
        CHECK(destination[2] == "three");
    }

    TEST_CASE(test_self_assignment) {
        TStackVector<int, 4> vector{1, 2, 3};

        auto* self = &vector;

        vector = *self;

        CHECK_EQ(vector.size(), 3);
        CHECK_EQ(vector[0], 1);
        CHECK_EQ(vector[1], 2);
        CHECK_EQ(vector[2], 3);

        vector = std::move(*self);

        CHECK_EQ(vector.size(), 3);
        CHECK_EQ(vector[0], 1);
        CHECK_EQ(vector[1], 2);
        CHECK_EQ(vector[2], 3);
    }

    TEST_CASE(test_move_only_type) {
        TStackVector<std::unique_ptr<int>, 3> source;

        source.push_back(std::make_unique<int>(10));
        source.emplace_back(std::make_unique<int>(20));

        TStackVector<std::unique_ptr<int>, 3> moved(std::move(source));

        CHECK(source.empty());
        CHECK_EQ(moved.size(), 2);
        CHECK_EQ(*moved[0], 10);
        CHECK_EQ(*moved[1], 20);
    }

    TEST_CASE(test_iterators) {
        TStackVector<int, 5> vector{10, 20, 30, 40};

        auto begin = vector.begin();
        auto end = vector.end();

        CHECK_EQ(1 + begin, begin + 1);

        CHECK_EQ(*begin, 10);
        CHECK_EQ(begin[1], 20);
        CHECK_EQ(*(begin + 2), 30);
        CHECK_EQ(*(end - 1), 40);

        CHECK_EQ(end - begin, 4);

        CHECK(begin < end);
        CHECK(begin <= end);
        CHECK((begin <=> end) <= 0);
        CHECK(end > begin);
        CHECK(end >= begin);
        CHECK((end <=> begin) >= 0);
        CHECK(begin != end);

        ++begin;
        CHECK_EQ(*begin, 20);

        begin += 2;
        CHECK_EQ(*begin, 40);

        --begin;
        CHECK_EQ(*begin, 30);

        begin -= 2;
        CHECK_EQ(*begin, 10);
    }

    TEST_CASE(test_const_iterators) {
        TStackVector<int, 4> vector{1, 2, 3};
        const auto& const_vector = vector;

        auto iterator = vector.begin();
        auto const_iterator = const_vector.begin();

        CHECK(iterator == const_iterator);
        CHECK_EQ(*const_iterator, 1);
        CHECK_EQ(const_vector.end() - const_vector.begin(), 3);

        STATIC_CHECK((
            std::is_same_v<
                decltype(*std::declval<const TStackVector<int, 4>&>().begin()),
                const int&
            >
        ));
    }

    TEST_CASE(test_reverse_iterators) {
        TStackVector<int, 4> vector{1, 2, 3, 4};

        auto iterator = vector.rbegin();

        CHECK_EQ(*iterator, 4);

        ++iterator;

        CHECK_EQ(*iterator, 3);
        CHECK_EQ(vector.rend() - vector.rbegin(), 4);

        const auto& const_vector = vector;

        CHECK_EQ(*const_vector.rbegin(), 4);
        CHECK_EQ(*const_vector.crbegin(), 4);
        CHECK_EQ(const_vector.crend() - const_vector.crbegin(), 4);
    }

    TEST_CASE(test_iterator_range_for) {
        TStackVector<int, 5> vector{1, 2, 3, 4, 5};

        int sum = 0;

        for (int value : vector) {
            sum += value;
        }

        CHECK_EQ(sum, 15);

        for (int& value : vector) {
            value *= 2;
        }

        CHECK_EQ(vector[0], 2);
        CHECK_EQ(vector[1], 4);
        CHECK_EQ(vector[2], 6);
        CHECK_EQ(vector[3], 8);
        CHECK_EQ(vector[4], 10);
    }

    TEST_CASE(test_storage_alignment) {
        TStackVector<TAlignedValue, 2> vector;

        vector.emplace_back(42);

        const auto address =
            reinterpret_cast<uintptr_t>(vector.data());

        CHECK_EQ(address % alignof(TAlignedValue), 0);
        CHECK_EQ(vector[0].value, 42);
    }

    TEST_CASE(test_copy_constructor_exception_safety) {
        using TVector = TStackVector<TCopyBomb, 4>;

        TCopyBomb::reset();

        {
            TVector source;

            source.emplace_back(1);
            source.emplace_back(2);
            source.emplace_back(3);

            CHECK_EQ(TCopyBomb::alive, 3);

            TCopyBomb::copies = 0;
            TCopyBomb::throw_on_copy = 2;

            CHECK_THROWS_AS(
                TVector(source),
                std::runtime_error
            );

            CHECK_EQ(TCopyBomb::alive, 3);
            CHECK_EQ(source.size(), 3);
            CHECK_EQ(source[0].value, 1);
            CHECK_EQ(source[1].value, 2);
            CHECK_EQ(source[2].value, 3);
        }

        CHECK_EQ(TCopyBomb::alive, 0);
    }

    TEST_CASE(test_copy_assignment_exception_safety) {
        using TVector = TStackVector<TCopyBomb, 4>;

        TCopyBomb::reset();

        {
            TVector source;

            source.emplace_back(1);
            source.emplace_back(2);
            source.emplace_back(3);

            TVector destination;

            destination.emplace_back(100);

            CHECK_EQ(TCopyBomb::alive, 4);

            TCopyBomb::copies = 0;
            TCopyBomb::throw_on_copy = 2;

            CHECK_THROWS_AS(
                destination = source,
                std::runtime_error
            );

            CHECK(destination.empty());
            CHECK_EQ(TCopyBomb::alive, 3);

            CHECK_EQ(source.size(), 3);
            CHECK_EQ(source[0].value, 1);
            CHECK_EQ(source[1].value, 2);
            CHECK_EQ(source[2].value, 3);
        }

        CHECK_EQ(TCopyBomb::alive, 0);
    }

} // namespace NCommon::NStruct::NTests
