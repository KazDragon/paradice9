set (munin_SOURCES
    text/default_multiline_document.cpp
    text/default_singleline_document.cpp
    text/document.cpp
    algorithm.cpp
    aligned_layout.cpp
    basic_component.cpp
    basic_container.cpp
    basic_frame.cpp
    button.cpp
    card.cpp
    clock.cpp
    compass_layout.cpp
    component.cpp
    composite_component.cpp
    container.cpp
    context.cpp
    dropdown_list.cpp
    edit.cpp
    filled_box.cpp
    frame.cpp
    framed_component.cpp
    grid_layout.cpp
    horizontal_scroll_bar.cpp
    horizontal_squeeze_layout.cpp
    horizontal_strip_layout.cpp
    image.cpp
    layout.cpp
    list.cpp
    named_frame.cpp
    rectangle.cpp
    scroll_pane.cpp
    solid_frame.cpp
    status_bar.cpp
    tabbed_frame.cpp
    tabbed_panel.cpp
    text_area.cpp
    toggle_button.cpp
    vertical_scroll_bar.cpp
    vertical_squeeze_layout.cpp
    vertical_strip_layout.cpp
    viewport.cpp
    window.cpp
)

add_library(munin ${munin_SOURCES})

set_target_properties(munin
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden)

target_include_directories(munin
    PRIVATE ${CMAKE_SOURCE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
           ${CMAKE_SOURCE_DIR}/terminalpp/include
)

target_link_libraries(munin
    INTERFACE terminalpp ${Boost_SIGNALS_LIBRARY}
)

#Remove when we move to signals2 or some other mechanism.
target_compile_definitions(munin
    PRIVATE BOOST_SIGNALS_NO_DEPRECATION_WARNING
)

target_compile_features(munin
    PRIVATE 
        cxx_auto_type
        cxx_strong_enums
        cxx_generic_lambdas
)

generate_export_header(munin
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/munin/export.hpp"
)
