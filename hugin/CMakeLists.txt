set (HUGIN_SOURCE_FILES
    src/account_creation_screen.cpp
    src/active_encounter_view.cpp
    src/beast_editor.cpp
    src/bestiary_page.cpp
    src/character_creation_screen.cpp
    src/character_selection_screen.cpp
    src/command_prompt.cpp
    src/delete_confirmation_dialog.cpp
    src/encounter_editor.cpp
    src/encounters_page.cpp
    src/gm_tools_screen.cpp
    src/intro_screen.cpp
    src/main_screen.cpp
    src/password_change_screen.cpp
    src/user_interface.cpp
    src/wholist.cpp
)

set (HUGIN_INCLUDE_FILES
    include/hugin/account_creation_screen.hpp
    include/hugin/active_encounter_view.hpp
    include/hugin/beast_editor.hpp
    include/hugin/bestiary_page.hpp
    include/hugin/character_creation_screen.hpp
    include/hugin/character_selection_screen.hpp
    include/hugin/command_prompt.hpp
    include/hugin/delete_confirmation_dialog.hpp
    include/hugin/encounter_editor.hpp
    include/hugin/encounters_page.hpp
    include/hugin/export.hpp
    include/hugin/gm_tools_screen.hpp
    include/hugin/intro_screen.hpp
    include/hugin/main_screen.hpp
    include/hugin/password_change_screen.hpp
    include/hugin/user_interface.hpp
    include/hugin/wholist.hpp
)

add_library(hugin
    ${HUGIN_SOURCE_FILES}
    ${HUGIN_INCLUDE_FILES}
)
    
target_include_directories(hugin
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        
    PRIVATE
        # TODO: FIX ME
        ${CMAKE_SOURCE_DIR}/paradice/include
)

set_target_properties(hugin
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)

target_link_libraries(hugin
    PUBLIC
        munin
        ${Boost_SIGNALS_LIBRARY}
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
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/hugin/export.hpp"
)
