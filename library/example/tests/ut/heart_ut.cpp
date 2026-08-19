#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <game/example/heart.h>

TEST_CASE(heart) {
    CHECK_EQ(game::example::heart(), "<3");
}

TEST_CASE(heart_message) {
    CHECK_EQ(game::example::heart_message(""), "<3");
    CHECK_EQ(game::example::heart_message("json"), "<3 json");
}
