#include "munin/canvas_view.hpp"

namespace munin {
// ==========================================================================
// ROW_PROXY::CONSTRUCTOR
// ==========================================================================
canvas_view::row_proxy::row_proxy(
    canvas_view& surface, 
    terminalpp::coordinate_type column, 
    terminalpp::coordinate_type row)
  : surface_(surface),
    column_(column),
    row_(row)
{
}

// ==========================================================================
// ROW_PROXY::OPERATOR=
// ==========================================================================
canvas_view::row_proxy &canvas_view::row_proxy::operator=(
    terminalpp::element const &value)
{
    surface_.set_element(column_, row_, value);
    return *this;
}

// ==========================================================================
// COLUMN_PROXY::CONSTRUCTOR
// ==========================================================================
canvas_view::column_proxy::column_proxy(
    canvas_view &surface, 
    terminalpp::coordinate_type column)
  : surface_(surface),
    column_(column)
{
}

// ==========================================================================
// COLUMN_PROXY::OPERATOR[]
// ==========================================================================
canvas_view::row_proxy canvas_view::column_proxy::operator[](
    terminalpp::coordinate_type row)
{
    return canvas_view::row_proxy(surface_, column_, row);
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas_view::canvas_view(terminalpp::canvas &cvs)
  : canvas_(cvs)
{
}

// ==========================================================================
// OFFSET_BY
// ==========================================================================
void canvas_view::offset_by(
    terminalpp::extent offset)
{
    offset_ += offset;
}

// ==========================================================================
// SIZE
// ==========================================================================
terminalpp::extent canvas_view::size() const
{
    return canvas_.size() - offset_;
}

// ==========================================================================
// OPERATOR[]
// ==========================================================================
canvas_view::column_proxy canvas_view::operator[](
    terminalpp::coordinate_type column)
{
    return column_proxy(*this, column);
}

// ==========================================================================
// SET_ELEMENT
// ==========================================================================
void canvas_view::set_element(
    terminalpp::coordinate_type column, 
    terminalpp::coordinate_type row, 
    terminalpp::element const &value)
{
    canvas_[column + offset_.width][row + offset_.height] = value;
}

}