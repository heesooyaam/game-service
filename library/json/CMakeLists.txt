add_library(json_value
    src/json_value.cpp
)


target_include_directories(json_value
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
