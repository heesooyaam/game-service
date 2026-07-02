add_library(game_game_core INTERFACE)

target_include_directories(game_game_core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game_game_core
    INTERFACE
        game_common
        game_json
)
