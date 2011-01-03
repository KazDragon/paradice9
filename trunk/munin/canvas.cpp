// ==========================================================================
// Munin Canvas.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file 
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor 
//    or contributors, which in any way restrict the ability of any party 
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
// ==========================================================================
#include "munin/canvas.hpp"
#include <boost/format.hpp>
#include <stdexcept>
#include <vector>

using namespace odin;
using namespace boost;
using namespace std;

namespace munin {

// ==========================================================================
// CANVAS IMPLEMENTATION STRUCTURE
// ==========================================================================
class canvas::impl
{
public :
    // ======================================================================
    // CONSTRUCTOR
    // ======================================================================
    impl()
        : offset_columns_(0)
        , offset_rows_(0)
    {
    }
    
    // ======================================================================
    // COPY CONSTRUCTOR
    // ======================================================================
    impl(impl const &other)
        : offset_columns_(other.offset_columns_)
        , offset_rows_(other.offset_rows_)
        , size_(other.size_)
        , elements_(other.elements_)
    {
    }
    
    // ======================================================================
    // OPERATOR=
    // ======================================================================
    impl &operator=(impl const &other)
    {
        size_     = other.size_;
        elements_ = other.elements_;
        return *this;
    }
    
    // ======================================================================
    // SET_SIZE
    // ======================================================================
    void set_size(extent const &size)
    {
        if (size_ == size)
        {
            // No change; no actions are necessary.
            return;
        }
        
        // Create a new elements array and fill it with data from the old
        // array if necessary or with default spaces.
        vector<element_type> new_elements;
        new_elements.resize(size.height * size.width);
        
        for (s32 row = 0; row < size.height; ++row)
        {
            for (s32 column = 0; column < size.width; ++column)
            {
                if (row < size_.height && column < size_.width)
                {
                    // This co-ordinate exists within the old buffer.  Copy
                    // the old element.
                    new_elements[(row * size.width) + column] =
                        elements_[(row * size_.width) + column];
                }
                else
                {
                    // This co-ordinate is a new element.  Create a default
                    // character.
                    new_elements[(row * size.width) + column] =
                        make_pair(' ', attribute());
                }
            }
        }
        
        // Set the new array as our array and the new size as our size.
        swap(elements_, new_elements);
        size_ = size;
    }

    // ======================================================================
    // GET_SIZE
    // ======================================================================
    extent get_size() const
    {
        return size_;
    }
    
    // ======================================================================
    // OPERATOR==
    // ======================================================================
    bool operator==(impl const &other) const
    {
        return size_     == other.size_
            && elements_ == other.elements_;
    }
    
    // ======================================================================
    // SET_VALUE
    // ======================================================================
    void set_value(s32 column, s32 row, element_type const &element)
    {
        column += offset_columns_;
        row    += offset_rows_;
        
        if (column >= size_.width || row >= size_.height)
        {
            throw std::out_of_range(str(format(
                "Access (write) of element (%d, %d) in canvas with extents "
                "[%d, %d]")
                    % column
                    % row
                    % size_.width
                    % size_.height));
        }
        
        elements_[(row * size_.width) + column] = element;
    }
    
    // ======================================================================
    // GET_VALUE
    // ======================================================================
    element_type get_value(s32 column, s32 row) const
    {
        column += offset_columns_;
        row    += offset_rows_;
        
        if (column >= size_.width || row >= size_.height)
        {
            throw std::out_of_range(str(format(
                "Access (read) of element (%d, %d) in canvas with extents "
                "[%d, %d]")
                    % column
                    % row
                    % size_.width
                    % size_.height));
        }
        
        return elements_[(row * size_.width) + column];
    }
    
    // ======================================================================
    // APPLY_OFFSET
    // ======================================================================
    void apply_offset(odin::s32 columns, odin::s32 rows)
    {
        offset_columns_ += columns;
        offset_rows_    += rows;
    }
    
private :
    s32                  offset_columns_;
    s32                  offset_rows_;
    extent               size_;
    vector<element_type> elements_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas::row_proxy::row_proxy(canvas &canvas, s32 column, s32 row)
    : canvas_(canvas)
    , column_(column)
    , row_(row)
{
}

// ==========================================================================
// OPERATOR=
// ==========================================================================
void canvas::row_proxy::operator=(element_type value)
{
    canvas_.pimpl_->set_value(column_, row_, value);
}

// ==========================================================================
// CONVERSION OPERATOR: ELEMENT TYPE
// ==========================================================================
canvas::row_proxy::operator element_type()
{
    return canvas_.pimpl_->get_value(column_, row_);
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas::column_proxy::column_proxy(canvas &canvas, s32 column)
    : canvas_(canvas)
    , column_(column)
{
}

// ==========================================================================
// OPERATOR[]
// ==========================================================================
canvas::row_proxy canvas::column_proxy::operator[](s32 row)
{
    return canvas::row_proxy(canvas_, column_, row);
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
canvas::canvas()
    : pimpl_(new impl)
{
}

// ==========================================================================
// COPY CONSTRUCTOR
// ==========================================================================
canvas::canvas(canvas const &other)
    : pimpl_(new impl(*other.pimpl_))
{
}

// ==========================================================================
// OPERATOR=
// ==========================================================================
canvas &canvas::operator=(canvas const &other)
{
    *pimpl_ = *other.pimpl_;
    return *this;
}

// ==========================================================================
// OPERATOR==
// ==========================================================================
bool canvas::operator==(canvas const &other) const
{
    return *pimpl_ == *other.pimpl_;
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
canvas::~canvas()
{
}

// ==========================================================================
// OPERATOR[]
// ==========================================================================
canvas::column_proxy canvas::operator[](s32 column)
{
    return column_proxy(*this, column);
}

// ==========================================================================
// SET_SIZE
// ==========================================================================
void canvas::set_size(extent const &size)
{
    pimpl_->set_size(size);
}

// ==========================================================================
// GET_SIZE
// ==========================================================================
extent canvas::get_size() const
{
    return pimpl_->get_size();
}

// ==========================================================================
// APPLY_OFFSET
// ==========================================================================
void canvas::apply_offset(odin::s32 columns, odin::s32 rows)
{
    pimpl_->apply_offset(columns, rows);
}

}
