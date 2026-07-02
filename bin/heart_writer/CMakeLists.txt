add_executable(heart_writer
    main.cpp
)

target_link_libraries(heart_writer
    PRIVATE
        game_example
)
