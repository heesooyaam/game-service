add_executable(common_ut
    version_ut.cpp
)

target_link_libraries(common_ut
    PRIVATE
        common
        test_framework
)

add_test(NAME common_ut COMMAND common_ut)
