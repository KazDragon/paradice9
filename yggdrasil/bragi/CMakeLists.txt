set (yggdrasil_bragi_SOURCES
    ansi/protocol.cpp
)

add_library(bragi ${yggdrasil_bragi_SOURCES})

target_include_directories(bragi
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_compile_features(bragi
    PUBLIC
        cxx_inline_namespaces
        cxx_strong_enums
)
