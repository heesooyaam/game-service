add_library(game_common
    src/version.cpp
)

target_include_directories(game_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
