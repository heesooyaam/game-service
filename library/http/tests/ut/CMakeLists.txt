add_executable(http_class_ut
    http_class_ut.cpp
)

add_executable(http_serializer_ut
    http_serializer_ut.cpp
)

target_link_libraries(http_class_ut
    PRIVATE
        http
        test_framework
)

target_link_libraries(http_serializer_ut
    PRIVATE
        http
        test_framework
)

add_test(NAME http_class_ut COMMAND http_class_ut)
add_test(NAME http_serializer_ut COMMAND http_serializer_ut)
