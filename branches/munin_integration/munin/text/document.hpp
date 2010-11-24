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
#ifndef MUNIN_TEXT_DOCUMENT_HPP_
#define MUNIN_TEXT_DOCUMENT_HPP_

#include "munin/types.hpp"
#include "odin/types.hpp"
#include "odin/runtime_array.hpp"
#include <boost/signal.hpp>

namespace munin { namespace text {

//* =========================================================================
/// \interface munin::text::document<CharacterType>
/// \tparam CharacterType the type of character that this document contains.
/// \brief Provides a document model for a text control.
//* =========================================================================
template <class CharacterType>
class document
{
public :
    typedef CharacterType           character_type;
    typedef document<CharacterType> document_type;

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    virtual ~document() 
    {
    }

    //* =====================================================================
    /// \brief Sets the document's width.
    //* =====================================================================
    void set_width(odin::u32 width)
    {
        do_set_width(width);
    }

    //* =====================================================================
    /// \brief Gets the document's width.
    //* =====================================================================
    odin::u32 get_width() const
    {
        return do_get_width();
    }

    //* =====================================================================
    /// \brief Sets the document's height.
    //* =====================================================================
    void set_height(odin::u32 height)
    {
        do_set_height(height);
    }

    //* =====================================================================
    /// \brief Gets the document's height.
    //* =====================================================================
    odin::u32 get_height() const
    {
        return do_get_height();
    }

    //* =====================================================================
    /// \brief Sets the caret's location from an x,y position.
    //* =====================================================================
    void set_caret_position(munin::point const& pt)
    {
        do_set_caret_position(pt);
        on_caret_position_changed();
    }

    //* =====================================================================
    /// \brief Returns the caret's location as an x,y position.
    //* =====================================================================
    munin::point get_caret_position() const
    {
        return do_get_caret_position();
    }

    //* =====================================================================
    /// \brief Sets the caret's location from an index into the text.
    //* =====================================================================
    void set_caret_index(odin::u32 index)
    {
        do_set_caret_index(index);
        on_caret_position_changed();
    }

    //* =====================================================================
    /// \brief Returns the caret's location as an index into the text.
    //* =====================================================================
    odin::u32 get_caret_index() const
    {
        return do_get_caret_index();
    }

    //* =====================================================================
    /// \brief Selects a region of text
    /// \param from The index of the character to select from.
    /// \param to Optionally, the index to select to.  If this is omitted,
    /// the parameter is left empty which indicates the end of the document
    /// by default.
    //* =====================================================================
    void select_text(
        odin::u32                  from
      , boost::optional<odin::u32> to = boost::optional<odin::u32>())
    {
        do_select_text(from, to);
    }
    
    //* =====================================================================
    /// \brief Optionally, returns the region of text that is currently 
    /// selected.
    //* =====================================================================
    boost::optional< std::pair<odin::u32, odin::u32> > 
        get_selected_text_region() const
    {
        return get_selected_text_region();
    }
    
    //* =====================================================================
    /// \brief Inserts the given characters at the caret.
    //* =====================================================================
    void insert_text(odin::runtime_array<character_type> const& text)
    {
        do_insert_text(text);
    }

    //* =====================================================================
    /// \brief If there is a region selected in the document, then that 
    /// region is deleted.  Otherwise, if there is a character immediately
    /// left of the caret, that character is deleted.
    //* =====================================================================
    void delete_text()
    {
        do_delete_text();
    }
    
    //* =====================================================================
    /// \brief Returns the number of lines in the text.
    //* =====================================================================
    odin::u32 get_number_of_lines() const
    {
        return do_get_number_of_lines();
    }

    //* =====================================================================
    /// \brief Returns the specified line of text in the document.
    //* =====================================================================
    odin::runtime_array<character_type> get_text_line(odin::u32 index) const
    {
        return do_get_text_line(index);
    }

    //* =====================================================================
    /// \fn on_redraw
    /// \param regions The regions of the document that require redrawing.
    /// \brief Connect to this signal in order to receive notifications about
    /// when the component should be redrawn.
    //* =====================================================================
    boost::signal
    <
        void (std::vector<rectangle> regions)
    > on_redraw;
    
    //* =====================================================================
    /// \fn on_caret_position_changed
    /// \brief Connect to this signal in order to receive notifications about
    /// when the caret has changed position.
    //* =====================================================================
    boost::signal
    <
        void ()
    > on_caret_position_changed;
    
private :
    //* =====================================================================
    /// \brief Called by set_width().  Derived classes must override this
    /// function in order to set the width of the document in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_width(odin::u32 width) = 0;

    //* =====================================================================
    /// \brief Called by get_width().  Derived classes must override this
    /// function in order to retrieve the width of the document in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_width() const = 0;

    //* =====================================================================
    /// \brief Called by set_height().  Derived classes must override this
    /// function in order to set the height of the document in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_height(odin::u32 height) = 0;

    //* =====================================================================
    /// \brief Called by get_height().  Derived classes must override this
    /// function in order to retrieve the height of the document in a
    /// custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_height() const = 0;

    //* =====================================================================
    /// \brief Called by set_caret_position().  Derived classes must
    /// override this function in order to set the caret's position in a
    /// custom manner.
    //* =====================================================================
    virtual void do_set_caret_position(munin::point const& pt) = 0;

    //* =====================================================================
    /// \brief Called by get_caret_position().  Derived classes must
    /// override this function in order to retrieve the caret's position in a
    /// custom manner.
    //* =====================================================================
    virtual munin::point do_get_caret_position() const = 0;

    //* =====================================================================
    /// \brief Called by set_caret_index().  Derived classes must
    /// override this function in order to set the caret's index in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_caret_index(odin::u32 index) = 0;

    //* =====================================================================
    /// \brief Called by get_caret_index().  Derived classes must override
    /// this function in order to retrieve the caret's position in a custom
    /// manner.
    //* =====================================================================
    virtual odin::u32 do_get_caret_index() const = 0;

    //* =====================================================================
    /// \brief Called by select_text().  Derived classes must override this
    /// function in order to select text in a custom manner.
    //* =====================================================================
    virtual void do_select_text(
        odin::u32                  from
      , boost::optional<odin::u32> to) = 0;
    
    //* =====================================================================
    /// \brief Called by get_selected_text().  Derived classes must override
    /// this function in order to retrieve the selected text in a custom
    /// manner.
    //* =====================================================================
    virtual boost::optional< std::pair<odin::u32, odin::u32> >
        do_get_selected_text_region() const = 0;

    //* =====================================================================
    /// \brief Called by insert_text().  Derived classes must override this
    /// function in order to insert text into the document in a custom
    /// manner.
    //* =====================================================================
    virtual void do_insert_text(
        odin::runtime_array<character_type> const& text) = 0;

    //* =====================================================================
    /// \brief Called by delete_text().  Derived classes must override this
    /// function in order to delete text in a custom manner.
    //* =====================================================================
    virtual void do_delete_text() = 0;

    //* =====================================================================
    /// \brief Called by get_number_of_lines().  Derived classes must 
    /// override this function in order to get the number of lines in the
    /// document in a custom manner.
    //* =====================================================================
    virtual odin::u32 do_get_number_of_lines() const = 0;

    //* =====================================================================
    /// \brief Called by get_text_line().  Derived classes must override this
    /// function in order to return the text line in a custom manner.
    //* =====================================================================
    virtual odin::runtime_array<character_type> do_get_text_line(
        odin::u32 index) const = 0;
};

}}

#endif
