cmake_minimum_required(VERSION 3.20)

project(turn_forge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(library/common)
add_subdirectory(library/json)
add_subdirectory(library/example)
add_subdirectory(library/game_core)
add_subdirectory(library/example2)
add_subdirectory(bin/gateway_service)
add_subdirectory(bin/heart_writer)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
