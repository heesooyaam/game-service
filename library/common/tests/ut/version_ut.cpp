#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/common/version.h>

TEST_CASE(version) {
    CHECK_EQ(game::common::version(), "0.1.0");
}
