set (yggdrasil_munin_SOURCES
    attribute.cpp
    basic_model.cpp
    canvas.cpp
    detail/string_to_elements.cpp
    element.cpp
    estring.cpp
    extent.cpp
    filled_box.cpp
    glyph.cpp
    image.cpp
    point.cpp
    ptree_model.cpp
    rectangle.cpp
)

#TODO: Rename this to munin when the old munin is obsolete.
add_library(yg_munin ${yggdrasil_munin_SOURCES})

target_include_directories(yg_munin
    PRIVATE
		${Boost_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}
)

target_compile_features(yg_munin
    PUBLIC
        cxx_inline_namespaces
        cxx_strong_enums
    PRIVATE
        cxx_return_type_deduction
)