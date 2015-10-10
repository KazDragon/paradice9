set (odin_SOURCES
    ansi/ansi_parser.cpp
    net/server.cpp
    net/socket.cpp
    tokenise.cpp
)

add_library(odin STATIC ${odin_SOURCES})

target_include_directories(odin 
    PRIVATE ${CMAKE_SOURCE_DIR}
)

target_compile_features(odin
    PUBLIC
        cxx_strong_enums
    PRIVATE
        cxx_auto_type
)

