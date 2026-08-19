add_executable(http_class_ut
    http_class_ut.cpp
)

target_link_libraries(http_class_ut
    PRIVATE
        http
        test_framework
)

add_test(NAME http_class_ut COMMAND http_class_ut)
