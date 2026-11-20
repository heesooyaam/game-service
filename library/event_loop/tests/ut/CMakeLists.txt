add_executable(event_loop_ut
    event_loop_ut.cpp
)

add_executable(socket_io_ut
    socket_io_ut.cpp
)

target_link_libraries(event_loop_ut
    PRIVATE
        event_loop
        common
        test_framework
)

target_link_libraries(socket_io_ut
    PRIVATE
        event_loop
        common
        test_framework
)

add_test(NAME event_loop_ut COMMAND event_loop_ut)
add_test(NAME socket_io_ut COMMAND socket_io_ut)
