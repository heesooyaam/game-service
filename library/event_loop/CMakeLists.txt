add_library(event_loop
    src/channel.cpp
    src/epoller.cpp
    src/event_fd.cpp
    src/event_loop.cpp
    src/fd.cpp
    src/error_io.cpp
    src/error_event_loop.cpp
)

target_include_directories(event_loop
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(event_loop
    PUBLIC
        common
)

add_subdirectory(tests/ut)
