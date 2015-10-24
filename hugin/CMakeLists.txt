set (hugin_SOURCES
    account_creation_screen.cpp
    active_encounter_view.cpp
    beast_editor.cpp
    bestiary_page.cpp
    character_creation_screen.cpp
    character_selection_screen.cpp
    command_prompt.cpp
    delete_confirmation_dialog.cpp
    encounter_editor.cpp
    encounters_page.cpp
    gm_tools_screen.cpp
    intro_screen.cpp
    main_screen.cpp
    password_change_screen.cpp
    user_interface.cpp
    wholist.cpp
)

add_library(hugin ${hugin_SOURCES})
    
target_include_directories(hugin
    PRIVATE ${CMAKE_SOURCE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
)

set_target_properties(hugin
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)

target_link_libraries(hugin
    INTERFACE munin
)

target_compile_definitions(hugin
    PRIVATE
        BOOST_SIGNALS_NO_DEPRECATION_WARNING
)

target_compile_features(hugin
    PRIVATE
        cxx_strong_enums
        cxx_generic_lambdas
)

generate_export_header(hugin
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/hugin/export.hpp"
)
