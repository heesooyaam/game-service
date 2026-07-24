add_executable(game_service
    main.cpp
)

target_link_libraries(game_service
    PRIVATE
        common
)
