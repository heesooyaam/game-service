set(
    TURN_FORGE_SANITIZER
    "none"
    CACHE STRING
    "Sanitizer profile: none, address-undefined, or thread"
)
set_property(
    CACHE TURN_FORGE_SANITIZER
    PROPERTY STRINGS none address-undefined thread
)

set(TURN_FORGE_SUPPORTED_SANITIZERS none address-undefined thread)
if(NOT TURN_FORGE_SANITIZER IN_LIST TURN_FORGE_SUPPORTED_SANITIZERS)
    message(FATAL_ERROR "Unsupported sanitizer profile: ${TURN_FORGE_SANITIZER}")
endif()

if(MSVC)
    add_compile_options(/W4 /WX)

    if(NOT TURN_FORGE_SANITIZER STREQUAL "none")
        message(FATAL_ERROR "Sanitizer profiles are supported only with GCC or Clang")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wformat=2
        -Werror
    )

    if(TURN_FORGE_SANITIZER STREQUAL "address-undefined")
        add_compile_options(
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all
        )
        add_link_options(
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all
        )
    elseif(TURN_FORGE_SANITIZER STREQUAL "thread")
        add_compile_options(
            -fsanitize=thread
            -fno-omit-frame-pointer
        )
        add_link_options(
            -fsanitize=thread
            -fno-omit-frame-pointer
        )
    endif()
else()
    message(WARNING "Strict warning flags are not configured for this compiler")
endif()
