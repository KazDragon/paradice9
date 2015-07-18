set (yggdrasil_munin_SOURCES
    attribute.cpp
    canvas.cpp
    detail/string_to_elements.cpp
    element.cpp
    estring.cpp
    extent.cpp
    filled_box.cpp
    glyph.cpp
    image.cpp
    point.cpp
    rectangle.cpp
)

#TODO: Rename this to munin when the old munin is obsolete.
add_library(yg_munin ${yggdrasil_munin_SOURCES})
