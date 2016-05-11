set (odin_SOURCES
    src/net/server.cpp
    src/net/socket.cpp
    src/tokenise.cpp
)

add_library(odin ${odin_SOURCES})

target_include_directories(odin 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
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
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/odin/export.hpp"
)
