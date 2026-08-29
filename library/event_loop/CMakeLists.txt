find_package(Threads REQUIRED)

add_library(event_loop
    src/channel.cpp
    src/event_loop.cpp
)

target_include_directories(event_loop
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(event_loop
    PUBLIC
        Threads::Threads
)

add_subdirectory(tests/ut)
