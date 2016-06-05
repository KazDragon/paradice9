set (munin_SOURCES
    src/text/default_multiline_document.cpp
    src/text/default_singleline_document.cpp
    src/text/document.cpp
    src/algorithm.cpp
    src/aligned_layout.cpp
    src/background_fill.cpp
    src/basic_component.cpp
    src/basic_container.cpp
    src/basic_frame.cpp
    src/button.cpp
    src/card.cpp
    src/clock.cpp
    src/compass_layout.cpp
    src/component.cpp
    src/composite_component.cpp
    src/container.cpp
    src/context.cpp
    src/dropdown_list.cpp
    src/edit.cpp
    src/filled_box.cpp
    src/frame.cpp
    src/framed_component.cpp
    src/grid_layout.cpp
    src/horizontal_scroll_bar.cpp
    src/horizontal_squeeze_layout.cpp
    src/horizontal_strip_layout.cpp
    src/image.cpp
    src/layout.cpp
    src/list.cpp
    src/named_frame.cpp
    src/rectangle.cpp
    src/scroll_pane.cpp
    src/solid_frame.cpp
    src/status_bar.cpp
    src/tabbed_frame.cpp
    src/tabbed_panel.cpp
    src/text_area.cpp
    src/toggle_button.cpp
    src/vertical_scroll_bar.cpp
    src/vertical_squeeze_layout.cpp
    src/vertical_strip_layout.cpp
    src/viewport.cpp
    src/window.cpp
)

add_library(munin ${munin_SOURCES})

set_target_properties(munin
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden)

target_include_directories(munin
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(munin
    PUBLIC
        odin
        terminalpp
        ${Boost_SIGNALS_LIBRARY}
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
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/include/munin/export.hpp"
)
