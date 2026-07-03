add_library(integer_sum 
    src/heart.cpp
)


target_include_directories(integer_sum
     PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
