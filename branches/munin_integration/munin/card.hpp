// ==========================================================================
// Munin Card.
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
#ifndef MUNIN_CARD_HPP_
#define MUNIN_CARD_HPP_

#include "munin/basic_component.hpp"
#include <boost/optional.hpp>
#include <boost/typeof/typeof.hpp>
#include <map>

namespace munin {

//* =========================================================================
/// \brief A class template that implements a component that can hold many
/// other components, known as "faces".  The card displays only one face at
/// a time, and can be commanded to show any of them.
//* =========================================================================
template <class ElementType>
class card : public basic_component<ElementType>
{
public :
    typedef ElementType                    element_type;
    typedef munin::component<element_type> component_type;

    //* =====================================================================
    /// \brief Adds a named face to the card.
    //* =====================================================================
    void add_face(
        boost::shared_ptr<component_type> const &component,
        std::string                       const &name)
    {
        faces_[name] = component;
    }

    //* =====================================================================
    /// \brief Returns the number of faces that this component contains.
    //* =====================================================================
    odin::u32 get_number_of_faces() const
    {
        return faces_.size();
    }

    //* =====================================================================
    /// \brief Selects a face for drawing.
    //* =====================================================================
    void select_face(std::string const &name)
    {
        boost::optional<std::string> old_face_ = current_face_;
        current_face_ = name;

        if (!old_face_.is_initialized() || old_face_.get() != current_face_)
        {
            // The face of the card has changed.  Fire an on_redraw event for
            // the entire component.
            munin::rectangle region(
                this->get_position()
              , this->get_size());
            vector<munin::rectangle> regions;
            regions.push_back(region);

            on_redraw(regions);
        }
    }

private :
    //* =====================================================================
    //  GET_CURRENT_FACE
    //* =====================================================================
    boost::optional< 
        boost::shared_ptr<component_type> 
    > get_current_face() const
    {
        if (current_face_.is_initialized())
        {
            face_map_type::const_iterator face_iter =
                faces_.find(current_face_.get());

            if (face_iter != faces_.end())
            {
                return face_iter->second;
            }
        }

        return NULL;
    }

    //* =====================================================================
    /// \brief Called by has_focus().  Derived classes must override this
    /// function in order to return whether this component has focus in a
    /// custom manner.
    //* =====================================================================
    virtual bool do_has_focus() const
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->has_focus();
        }

        return false;
    }

    //* =====================================================================
    /// \brief Called by can_focus().  Derived classes must override this
    /// function in order to return whether this component can be focused in
    /// a custom manner.
    //* =====================================================================
    virtual bool do_can_focus() const
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->can_focus();
        }

        return false;
    }

    //* =====================================================================
    /// \brief Called by set_focus().  Derived classes must override this
    /// function in order to set the focus to this component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_focus()
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->set_focus();
            on_focus_set();
        }
    }

    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual extent do_get_preferred_size() const
    {
        return extent();
    }

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw 
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.  That is, (0,0) to this component
    ///        is actually (offset.x, offset.y) in the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas<element_type> &cvs
      , point const          &offset
      , rectangle const      &region)
    {
        if (current_face_.is_initialized())
        {
            std::string current_face = current_face_.get();

            face_map_type::const_iterator face_iter = faces_.find(current_face);

            if (face_iter != faces_.end())
            {
                face_iter->second->draw(cvs, offset, region);
            }
        }
    }

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
    {
    }

    typedef std::map<
        std::string
      , boost::shared_ptr<component_type> 
    > face_map_type;

    face_map_type                faces_;
    boost::optional<std::string> current_face_;
};

}

#endif
