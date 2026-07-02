add_executable(storage_service
    main.cpp
)

target_link_libraries(storage_service
    PRIVATE
        game_common
)
