set (ODIN_SOURCE_FILES
    src/net/server.cpp
    src/net/socket.cpp
    src/tokenise.cpp
)

set (ODIN_INCLUDE_FILES
    include/odin/core.hpp
    include/odin/export.hpp
    include/odin/tokenise.hpp
    include/odin/io/datastream.hpp
    include/odin/io/input_datastream.hpp
    include/odin/io/output_datastream.hpp
    include/odin/net/server.hpp
    include/odin/net/socket.hpp
)

add_library(odin
    ${ODIN_SOURCE_FILES}
    ${ODIN_INCLUDE_FILES}
)

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
