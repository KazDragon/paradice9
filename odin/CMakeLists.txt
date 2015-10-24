set (odin_SOURCES
    ansi/ansi_parser.cpp
    net/server.cpp
    net/socket.cpp
    tokenise.cpp
)

add_library(odin ${odin_SOURCES})

target_include_directories(odin 
    PRIVATE ${CMAKE_SOURCE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
)

target_compile_features(odin
    PUBLIC
        cxx_strong_enums
    PRIVATE
        cxx_auto_type
)

set_target_properties(odin
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)

generate_export_header(odin
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/odin/export.hpp"
)