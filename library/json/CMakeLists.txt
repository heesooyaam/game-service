add_library(game_json
    src/json_value.cpp
)

target_include_directories(game_json
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
