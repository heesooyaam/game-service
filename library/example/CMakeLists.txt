add_library(game_example
    src/heart.cpp
)

target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
