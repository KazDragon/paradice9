#pragma once

#include "munin/export.hpp"
#include <terminalpp/canvas.hpp>

namespace munin {

//* =========================================================================
/// \brief A sub-view into a canvas that only allows reading and writing
/// to a its elements; not operations that would affect the entire canvas.
//* =========================================================================
class MUNIN_EXPORT canvas_view
{
public :
    //* =====================================================================
    /// \brief A proxy into a row of elements on the canvas
    //* =====================================================================
    class row_proxy
    {
    public :
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        row_proxy(
            canvas_view &surface, 
            terminalpp::coordinate_type column, 
            terminalpp::coordinate_type row);

        // ==================================================================
        // OPERATOR=
        // ==================================================================
        row_proxy &operator=(terminalpp::element const &value);

    private :
        canvas_view &surface_;
        terminalpp::coordinate_type column_;
        terminalpp::coordinate_type row_;
    };

    //* =====================================================================
    /// \brief A proxy into a column of elements on the canvas
    //* =====================================================================
    class column_proxy
    {
    public :
        // ==================================================================
        // CONSTRUCTOR
        // ==================================================================
        column_proxy(
            canvas_view &surface, 
            terminalpp::coordinate_type column);

        // ==================================================================
        // OPERATOR[]
        // ==================================================================
        row_proxy operator[](terminalpp::coordinate_type row);

    private :
        canvas_view &surface_;
        terminalpp::coordinate_type column_;
    };

    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit canvas_view(terminalpp::canvas &cvs);

    //* =====================================================================
    /// \brief Offsets the canvas by a certain amount, causing it to become
    /// a view with the offset location as a basis.
    //* =====================================================================
    void offset_by(terminalpp::extent offset);

    //* =====================================================================
    /// \brief Returns the size of the canvas.
    //* =====================================================================
    terminalpp::extent size() const;

    //* =====================================================================
    /// \brief A subscript operator into a column
    //* =====================================================================
    column_proxy operator[](terminalpp::coordinate_type column);

private :
    //* =====================================================================
    /// \brief Set the value of an element.
    //* =====================================================================
    void set_element(
        terminalpp::coordinate_type column, 
        terminalpp::coordinate_type row, 
        terminalpp::element const &value);

    terminalpp::canvas &canvas_;
    terminalpp::extent  offset_;
};

}