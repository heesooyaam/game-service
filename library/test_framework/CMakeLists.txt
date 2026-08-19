add_library(test_framework INTERFACE)

target_include_directories(test_framework
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
