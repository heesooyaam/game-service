add_executable(game_example_ut
    heart_ut.cpp
)

target_link_libraries(game_example_ut
    PRIVATE
        game_example
)

add_test(NAME game_example_ut COMMAND game_example_ut)
