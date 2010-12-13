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
#include <boost/foreach.hpp>
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

        // Connect the underlying container's default signals to the signals
        // of this component with.
        component->on_redraw.connect(
            boost::bind(boost::ref(this->on_redraw), _1));

        component->on_position_changed.connect(
            boost::bind(boost::ref(this->on_position_changed), _1, _2));

        component->on_focus_set.connect(
            boost::bind(boost::ref(this->on_focus_set)));

        component->on_focus_lost.connect(
            boost::bind(boost::ref(this->on_focus_lost)));
        
        component->on_cursor_state_changed.connect(
            boost::bind(boost::ref(this->on_cursor_state_changed), _1));

        component->on_cursor_position_changed.connect(
            boost::bind(boost::ref(this->on_cursor_position_changed), _1));
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
        BOOST_AUTO(old_face,      get_current_face());
        BOOST_AUTO(old_face_name, current_face_);

        current_face_ = name;

        BOOST_AUTO(new_face, get_current_face());

        if (!old_face_name.is_initialized() 
         || old_face_name.get() != current_face_)
        {
            // The name was initialised, so there must be a face to go with 
            // it.

            // Determine any focus change between the faces.
            bool old_focussed = old_face.is_initialized()
                              ? old_face.get()->has_focus()
                              : false;

            bool new_focussed = new_face.is_initialized()
                              ? new_face.get()->has_focus()
                              : false;

            // Determine any cursor state change
            bool old_cursor_state = old_face.is_initialized()
                                  ? old_face.get()->get_cursor_state()
                                  : false;

            bool new_cursor_state = new_face.is_initialized()
                                  ? new_face.get()->get_cursor_state()
                                  : false;

            point cursor_position = new_face.is_initialized()
                                  ? new_face.get()->get_cursor_position()
                                  : point();

            // Fire any necessary events.
            if (old_focussed != new_focussed)
            {
                if (new_focussed)
                {
                    this->on_focus_set();
                }
                else
                {
                    this->on_focus_lost();
                }
            }

            if (old_cursor_state != new_cursor_state)
            {
                this->on_cursor_state_changed(new_cursor_state);
            }

            this->on_cursor_position_changed(cursor_position);

            // The face of the card has changed.  Fire an on_redraw event for
            // the entire component.
            munin::rectangle region(munin::point(), this->get_size());
            std::vector<munin::rectangle> regions;
            regions.push_back(region);

            this->on_redraw(regions);
        }
    }

protected :
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
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        canvas<element_type> &cvs
      , rectangle const      &region)
    {
        if (current_face_.is_initialized())
        {
            std::string current_face = current_face_.get();

            typename face_map_type::const_iterator face_iter = 
                faces_.find(current_face);

            if (face_iter != faces_.end())
            {
                munin::point position = this->get_position();
                face_iter->second->draw(cvs, region);
            }
        }
    }

    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
    {
        if (current_face_.is_initialized())
        {
            std::string current_face = current_face_.get();

            typename face_map_type::const_iterator face_iter = 
                faces_.find(current_face);

            if (face_iter != faces_.end())
            {
                face_iter->second->event(event);
            }
        }
    }

    //* =====================================================================
    /// \brief Called by set_size().  Derived classes must override this 
    /// function in order to set the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual void do_set_size(extent const &size) 
    {
        basic_component<ElementType>::do_set_size(size);

        BOOST_FOREACH(typename face_map_type::value_type entry, faces_)
        {
            entry.second->set_size(size);
        }
    }

    //* =====================================================================
    /// \brief Called by focus_next().  Derived classes must override this
    /// function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_next() 
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->focus_next();
        }
    }
    
    //* =====================================================================
    /// \brief Called by focus_previous().  Derived classes must override 
    /// this function in order to move the focus in a custom manner.
    //* =====================================================================
    virtual void do_focus_previous() 
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->focus_previous();
        }
    }
    
    //* =====================================================================
    /// \brief Called by get_focussed_component().  Derived classes must
    /// override this function in order to return the focussed component
    /// in a custom manner.
    //* =====================================================================
    virtual boost::shared_ptr<component_type> do_get_focussed_component() 
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->get_focussed_component();
        }

        return boost::shared_ptr<component_type>();
    }
    
    //* =====================================================================
    /// \brief Called by enable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_enable()
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->enable();
        }
    }
    
    //* =====================================================================
    /// \brief Called by disable().  Derived classes must override this
    /// function in order to disable the component in a custom manner.
    //* =====================================================================
    virtual void do_disable()
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->disable();
        }
    }
    
    //* =====================================================================
    /// \brief Called by is_enabled().  Derived classes must override this
    /// function in order to return whether the component is disabled or not
    /// in a custom manner.
    //* =====================================================================
    virtual bool do_is_enabled() const
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->is_enabled();
        }

        return false;
    }

    //* =====================================================================
    /// \brief Called by get_cursor_state().  Derived classes must override
    /// this function in order to return the cursor state in a custom manner.
    //* =====================================================================
    virtual bool do_get_cursor_state() const 
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->get_cursor_state();
        }
        
        return false;
    }
    
    //* =====================================================================
    /// \brief Called by get_cursor_position().  Derived classes must
    /// override this function in order to return the cursor position in
    /// a custom manner.
    //* =====================================================================
    virtual point do_get_cursor_position() const 
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            return current_face.get()->get_cursor_position();
        }

        return point();
    }

    //* =====================================================================
    /// \brief Called by set_attribute().  Derived classes must override this
    /// function in order to set an attribute in a custom manner.
    //* =====================================================================
    virtual void do_set_attribute(
        std::string const &name, boost::any const &attr)
    {
        BOOST_AUTO(current_face, get_current_face());

        if (current_face.is_initialized())
        {
            current_face.get()->set_attribute(name, attr);
        }
    }

    //* =====================================================================
    //  GET_CURRENT_FACE
    //* =====================================================================
    boost::optional< 
        boost::shared_ptr<component_type> 
    > get_current_face() const
    {
        if (current_face_.is_initialized())
        {
            typename face_map_type::const_iterator face_iter =
                faces_.find(current_face_.get());

            if (face_iter != faces_.end())
            {
                return face_iter->second;
            }
        }

        return NULL;
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
