if (NOT TARGET KazDragon::terminalpp)
    find_package(terminalpp 4.0.2 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::munin)
    find_package(munin 0.8.1 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::telnetpp)
    find_package(telnetpp 4.0.0 CONFIG REQUIRED)
endif()

if (NOT TARGET KazDragon::consolepp)
    find_package(consolepp 0.1.1 QUIET) # Used if munin is built with MUNIN_WITH_CONSOLEPP=ON.
endif()
find_package(Boost COMPONENTS program_options container REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

set (PARADICE_SOURCE_FILES
    src/admin_commands.cpp
    src/client.cpp
    src/command_catalog.cpp
    src/connection.cpp
    src/dice.cpp
    src/dice_roll_parser.cpp

    src/ui/components/password_edit.cpp
    src/ui/components/roster.cpp
    src/ui/pages/account_creation_page.cpp
    src/ui/pages/character_creation_page.cpp
    src/ui/pages/character_selection_page.cpp
    src/ui/components/command_prompt.cpp
    src/ui/pages/main_page.cpp
    src/ui/pages/title_page.cpp
    src/ui/shell/user_interface.cpp
)

set (PARADICE_INCLUDE_FILES
    include/paradice/admin_commands.hpp
    include/paradice/client.hpp
    include/paradice/command_catalog.hpp
    include/paradice/connection.hpp
    include/paradice/context.hpp
    include/paradice/dice.hpp
    include/paradice/dice_roll_parser.hpp
    include/paradice/permissions.hpp
    include/paradice/room_lifecycle.hpp
    include/paradice/export.hpp

    include/paradice/model/account.hpp
    include/paradice/model/character.hpp
    include/paradice/model/room_membership.hpp
    
    include/paradice/ui/components/password_edit.hpp
    include/paradice/ui/components/roster.hpp
    include/paradice/ui/pages/account_creation_page.hpp
    include/paradice/ui/pages/character_creation_page.hpp
    include/paradice/ui/pages/character_selection_page.hpp
    include/paradice/ui/components/command_prompt.hpp
    include/paradice/ui/pages/main_page.hpp
    include/paradice/ui/pages/title_page.hpp
    include/paradice/ui/shell/user_interface.hpp
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
        KazDragon::terminalpp
        Boost::boost
        Boost::program_options
    PRIVATE
        KazDragon::munin
        KazDragon::telnetpp
        ZLIB::ZLIB
)


generate_export_header(paradice
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/paradice/export.hpp"
)
