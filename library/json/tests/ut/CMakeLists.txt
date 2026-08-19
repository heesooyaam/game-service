add_executable(json_value_ut
    json_value_ut.cpp
)

add_executable(json_parser_ut
    json_parser_ut.cpp
)

add_executable(json_serializer_ut
    json_serializer_ut.cpp
)

add_executable(json_round_trip_ut
    json_round_trip_ut.cpp
)

target_link_libraries(json_value_ut
    PRIVATE
        json
        test_framework
)

target_link_libraries(json_parser_ut
    PRIVATE
        json
        test_framework
)

target_link_libraries(json_serializer_ut
    PRIVATE
        json
        test_framework
)

target_link_libraries(json_round_trip_ut
    PRIVATE
        json
        test_framework
)

add_test(NAME json_value_ut COMMAND json_value_ut)
add_test(NAME json_parser_ut COMMAND json_parser_ut)
add_test(NAME json_serializer_ut COMMAND json_serializer_ut)
add_test(NAME json_round_trip_ut COMMAND json_round_trip_ut)
