add_library(test_framework
    src/test.cpp
)

target_include_directories(test_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
