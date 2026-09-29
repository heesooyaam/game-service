add_executable(common_ut
    version_ut.cpp
)

add_executable(struct_ut
    stack_vector_ut.cpp
)

target_link_libraries(common_ut
    PRIVATE
        common
        test_framework
)

target_link_libraries(struct_ut
    PRIVATE
        common
        test_framework
)

add_test(NAME common_ut COMMAND common_ut)
add_test(NAME struct_ut COMMAND struct_ut)
