add_library(event_loop
    src/channel.cpp
    src/epoller.cpp
    src/event_fd.cpp
    src/event_loop.cpp
    src/fd.cpp
    src/socket.cpp

)

target_include_directories(event_loop
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
