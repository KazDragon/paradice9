set (munin_SOURCES
    ansi/protocol.cpp
    text/default_multiline_document.cpp
    text/default_singleline_document.cpp
    text/document.cpp
    algorithm.cpp
    aligned_layout.cpp
    attribute.cpp
    basic_component.cpp
    basic_container.cpp
    basic_frame.cpp
    button.cpp
    canvas.cpp
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
    glyph.cpp
    grid_layout.cpp
    horizontal_scroll_bar.cpp
    horizontal_squeeze_layout.cpp
    horizontal_strip_layout.cpp
    image.cpp
    layout.cpp
    list.cpp
    named_frame.cpp
    scroll_pane.cpp
    solid_frame.cpp
    status_bar.cpp
    tabbed_frame.cpp
    tabbed_panel.cpp
    text_area.cpp
    toggle_button.cpp
    types.cpp
    vertical_scroll_bar.cpp
    vertical_squeeze_layout.cpp
    vertical_strip_layout.cpp
    viewport.cpp
    window.cpp
    )

add_library(munin 
    STATIC ${munin_SOURCES})

target_include_directories(munin
    PRIVATE ${CMAKE_SOURCE_DIR}
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
    
if (COMMAND cotire)
    cotire(munin)
endif()

