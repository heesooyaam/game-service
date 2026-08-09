add_library(http
    src/error.cpp
    src/headers.cpp
    src/request.cpp
    src/response.cpp
    src/version.cpp
)

target_include_directories(http
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(http
    PUBLIC
        common
)

add_subdirectory(tests/ut)
