add_executable(json_value_ut
    json_value_ut.cpp
)

target_link_libraries(json_value_ut
    PRIVATE
        json
)

add_test(NAME json_value_ut COMMAND json_value_ut)
