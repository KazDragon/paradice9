// ==========================================================================
// Munin Text Document.
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
#ifndef MUNIN_ANSI_TEXT_DEFAULT_SINGLELINE_DOCUMENT_HPP_
#define MUNIN_ANSI_TEXT_DEFAULT_SINGLELINE_DOCUMENT_HPP_

#include "munin/text/document.hpp"
#include "munin/ansi/ansi_types.hpp"

namespace munin { namespace ansi { namespace text {

//* =========================================================================
/// \brief Provides a document model for a single-lined ANSI text control.
//* =========================================================================
class default_singleline_document
    : public munin::text::document<munin::ansi::element_type>
{
public :
    typedef munin::ansi::element_type   character_type;
    typedef default_singleline_document document_type;

private :
    //* =====================================================================
    /// \brief Called by set_width().  Derived classes must override this
    /// function in order to set the width of the document in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_width(odin::u32 width);

    //* =====================================================================
    /// \brief Called by get_width().  Derived classes must override this
    /// function in order to retrieve the width of the document in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_width();

    //* =====================================================================
    /// \brief Called by set_height().  Derived classes must override this
    /// function in order to set the height of the document in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_height(odin::u32 height);

    //* =====================================================================
    /// \brief Called by get_height().  Derived classes must override this
    /// function in order to retrieve the height of the document in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_height();

    //* =====================================================================
    /// \brief Called by set_caret_position().  Derived classes must
    /// override this function in order to set the caret's position in a
    /// custom manner.
    //* =====================================================================
    virtual void do_set_caret_position(munin::point const& pt);

    //* =====================================================================
    /// \brief Called by get_caret_position().  Derived classes must
    /// override this function in order to retrieve the caret's position in a
    /// custom manner.
    //* =====================================================================
    virtual munin::point do_get_caret_position();

    //* =====================================================================
    /// \brief Called by set_caret_index().  Derived classes must
    /// override this function in order to set the caret's index in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_caret_index(odin::u32 index);

    //* =====================================================================
    /// \brief Called by get_caret_index().  Derived classes must override
    /// this function in order to retrieve the caret's position in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_caret_index();

    //* =====================================================================
    /// \brief Called by insert_text().  Derived classes must override this
    /// function in order to insert text into the document in a custom
    /// manner.
    //* =====================================================================
    virtual void do_insert_text(
        odin::runtime_array<character_type> const& text);
};

}}}

#endif
