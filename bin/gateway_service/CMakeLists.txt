add_executable(gateway_service
    main.cpp
)

target_link_libraries(gateway_service
    PRIVATE
        game_common
)
