set (odin_SOURCES
    ansi/ansi_parser.cpp
    net/server.cpp
    net/socket.cpp
    telnet/client_option.cpp
    telnet/command_router.cpp
    telnet/input_visitor.cpp
    telnet/negotiation_router.cpp
    telnet/protocol.cpp
    telnet/server_option.cpp
    telnet/stream.cpp
    telnet/subnegotiation_router.cpp
    telnet/detail/parser.cpp
    telnet/options/naws_client.cpp
    telnet/options/terminal_type_client.cpp
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

set_source_files_properties(telnet/stream.cpp
    PROPERTIES
        COTIRE_EXCLUDED TRUE
)

if (COMMAND cotire)
    cotire(odin)
endif()

