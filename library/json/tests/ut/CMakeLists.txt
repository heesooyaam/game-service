add_executable(json_value_ut
    json_value_ut.cpp
)

add_executable(json_parser_ut
    json_parser_ut.cpp
)

target_link_libraries(json_value_ut
    PRIVATE
        json
)

target_link_libraries(json_parser_ut
    PRIVATE
        json
)

add_test(NAME json_value_ut COMMAND json_value_ut)
add_test(NAME json_parser_ut COMMAND json_parser_ut)
