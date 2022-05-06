if (PARADICE9_USE_CONAN)
    set(PARADICE_PUBLIC_LIBRARIES 
        CONAN_PKG::terminalpp
        CONAN_PKG::boost
    )

    set(PARADICE_PRIVATE_LIBRARIES
        CONAN_PKG::munin
        CONAN_PKG::telnetpp
    )
else()
    find_package(terminalpp 2.2.0 REQUIRED)
    find_package(munin 0.3.11 REQUIRED)
    find_package(telnetpp REQUIRED)
    find_package(Boost COMPONENTS container program_options REQUIRED)
    find_package(gsl-lite 0.34 REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(fmt REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(Threads REQUIRED)

    set(PARADICE_PUBLIC_LIBRARIES
        KazDragon::terminalpp
        Boost::boost
        Boost::program_options
    )

    set(PARADICE_PRIVATE_LIBRARIES
        KazDragon::munin
        KazDragon::telnetpp
        ZLIB::ZLIB)
endif()


set (PARADICE_SOURCE_FILES
#    src/account.cpp
#    src/active_encounter.cpp
#    src/admin.cpp
#    src/beast.cpp
#    src/character.cpp
    src/client.cpp
#    src/communication.cpp
#    src/configuration.cpp
    src/connection.cpp
#    src/dice_roll_parser.cpp
#    src/encounter.cpp
#    src/gm.cpp
#    src/help.cpp
#    src/random.cpp
#    src/rules.cpp
#    src/tokenise.cpp
#    src/utility.cpp
#    src/who.cpp

    src/ui/detail/password_edit.cpp
    src/ui/account_creation_page.cpp
    src/ui/character_creation_page.cpp
    src/ui/character_selection_page.cpp
    src/ui/main_page.cpp
    src/ui/title_page.cpp
    src/ui/user_interface.cpp
)

set (PARADICE_INCLUDE_FILES
#    include/paradice/account.hpp
#    include/paradice/active_encounter.hpp
#    include/paradice/admin.hpp
#    include/paradice/beast.hpp
#    include/paradice/character.hpp
    include/paradice/client.hpp
#    include/paradice/command.hpp
#    include/paradice/communication.hpp
#    include/paradice/configuration.hpp
    include/paradice/connection.hpp
    include/paradice/context.hpp
#    include/paradice/dice.hpp
#    include/paradice/dice_roll_parser.hpp
#    include/paradice/encounter.hpp
    include/paradice/export.hpp
#    include/paradice/gm.hpp
#    include/paradice/help.hpp
#    include/paradice/random.hpp
#    include/paradice/rules.hpp
#    include/paradice/tokenise.hpp
#    include/paradice/utility.hpp
#    include/paradice/who.hpp

    include/paradice/model/account.hpp
    include/paradice/model/character.hpp
    
    include/paradice/ui/detail/password_edit.hpp
    include/paradice/ui/account_creation_page.hpp
    include/paradice/ui/character_creation_page.hpp
    include/paradice/ui/character_selection_page.hpp
    include/paradice/ui/main_page.hpp
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

target_link_libraries(paradice
    PUBLIC
        ${PARADICE_PUBLIC_LIBRARIES}
    PRIVATE
        ${PARADICE_PRIVATE_LIBRARIES}
)


generate_export_header(paradice
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/paradice/export.hpp"
)

