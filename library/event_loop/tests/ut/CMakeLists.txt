add_executable(event_loop_ut
    event_loop_ut.cpp
)

target_link_libraries(event_loop_ut
    PRIVATE
        event_loop
        test_framework
)

add_test(NAME event_loop_ut COMMAND event_loop_ut)
