add_library(json
    src/json_value.cpp
    src/json_parser.cpp
    src/json_serializer.cpp
    src/error.cpp
)

target_include_directories(json
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(json
    PUBLIC
        common
)

add_subdirectory(tests/ut)
