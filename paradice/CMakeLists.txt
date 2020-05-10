set (PARADICE_SOURCE_FILES
    src/account.cpp
    #src/active_encounter.cpp
    src/admin.cpp
    #src/beast.cpp
    src/character.cpp
    src/client.cpp
    src/communication.cpp
    src/configuration.cpp
    src/connection.cpp
    src/cryptography.cpp
    src/dice_roll_parser.cpp
    #src/encounter.cpp
    #src/gm.cpp
    src/help.cpp
    src/random.cpp
    src/rules.cpp
    src/tokenise.cpp
    src/utility.cpp
    src/who.cpp
    src/ui/title_page.cpp
    src/ui/user_interface.cpp
)

set (PARADICE_INCLUDE_FILES
    include/paradice/account.hpp
    #include/paradice/active_encounter.hpp
    include/paradice/admin.hpp
    #include/paradice/beast.hpp
    include/paradice/character.hpp
    include/paradice/client.hpp
    include/paradice/command.hpp
    include/paradice/communication.hpp
    include/paradice/configuration.hpp
    include/paradice/connection.hpp
    include/paradice/context.hpp
    include/paradice/cryptography.hpp
    include/paradice/dice.hpp
    include/paradice/dice_roll_parser.hpp
    #include/paradice/encounter.hpp
    include/paradice/export.hpp
    #include/paradice/gm.hpp
    include/paradice/help.hpp
    include/paradice/random.hpp
    include/paradice/rules.hpp
    include/paradice/tokenise.hpp
    include/paradice/utility.hpp
    include/paradice/who.hpp
    include/paradice/ui/title_page.hpp
    include/paradice/ui/user_interface.hpp
)

add_library(paradice
    ${PARADICE_SOURCE_FILES}
    ${PARADICE_INCLUDE_FILES}
)

set_target_properties(paradice
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)

target_include_directories(paradice
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Select the best cryptography available.
if (PARADICE_USE_CRYPTOPP)
    target_link_libraries(paradice
        PRIVATE 
            cryptopp)
else()
    target_compile_definitions(paradice
        PRIVATE
            PARADICE_NOCRYPT)
endif()


target_link_libraries(paradice
    PUBLIC
        CONAN_PKG::terminalpp
        CONAN_PKG::serverpp
    PRIVATE
        CONAN_PKG::munin
        CONAN_PKG::boost_uuid
        CONAN_PKG::telnetpp
)


generate_export_header(paradice
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/paradice/export.hpp"
)

