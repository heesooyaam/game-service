add_executable(calculator_ut
    heart_ut.cpp
)

target_link_libraries(calculator_ut
    PRIVATE
        integer_sum
        test_framework
)

add_test(NAME calculator_ut COMMAND calculator_ut)
