#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <game/example2/heart.h>

TEST_CASE(sum) {
    CHECK_EQ(game::example2::sum(1, 2), 3);
    CHECK_EQ(game::example2::sum(1, 3), 4);
    CHECK_EQ(game::example2::sum(67, 0), 67);
}
