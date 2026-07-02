add_executable(game_json_ut
    json_value_ut.cpp
)

target_link_libraries(game_json_ut
    PRIVATE
        game_json
)

add_test(NAME game_json_ut COMMAND game_json_ut)
