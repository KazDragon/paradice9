#include "munin/basic_component.hpp"
#include "munin/canvas.hpp"
#include "munin/types.hpp"
#include "munin/algorithm.hpp"
#include <algorithm>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

//#define DEBUG_COMPONENT

#ifdef DEBUG_COMPONENT
#include <iostream>
#endif

template <class ElementType>
class fake_component : public munin::basic_component<ElementType>
{
public :
    fake_component()
        : brush_()
        , can_focus_(true)
    {
        preferred_size_.width  = 0;
        preferred_size_.height = 0;
    }
    
    void set_brush(ElementType brush)
    {
        brush_ = brush;
        
        munin::rectangle region;
        region.origin.x = 0;
        region.origin.y = 0;
        region.size = this->get_size();
        
        std::vector<munin::rectangle> regions;
        regions.push_back(region);
        
        this->on_redraw(regions);
    }
    
    void set_preferred_size(munin::extent const &preferred_size)
    {
        preferred_size_ = preferred_size;
    }
    
    void set_can_focus(bool focus)
    {
        can_focus_ = focus;
    }
    
    void set_event_handler(boost::function<void (boost::any)> handler)
    {
        event_handler_ = handler;
    }
    
private :
    ElementType                        brush_;
    munin::extent                      preferred_size_;
    bool                               can_focus_;
    boost::function<void (boost::any)> event_handler_;
    
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom 
    /// manner.
    //* =====================================================================
    virtual munin::extent do_get_preferred_size() const
    {
        return preferred_size_;
    }
    
    //* =====================================================================
    /// \brief Called by can_focus().  Derived classes must override this
    /// function in order to return whether this component can be focused in
    /// a custom manner.
    //* =====================================================================
    virtual bool do_can_focus() const
    {
        return can_focus_;
    }
    
    //* =====================================================================
    /// \brief Called by event().  Derived classes must override this 
    /// function in order to handle events in a custom manner.
    //* =====================================================================
    virtual void do_event(boost::any const &event)
    {
        event_handler_(event);
    }
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed canvas.  A component must only draw
    /// the part of itself specified by the region.
    ///
    /// \param cvs the canvas in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the canvas.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        munin::canvas<ElementType> &cvs
      , munin::point const         &offset
      , munin::rectangle const     &region)
    {
        munin::rectangle bounds;
        bounds.origin = this->get_position();
        bounds.size   = this->get_size();
        
#ifdef DEBUG_COMPONENT
        std::cout << "fake_component::do_draw" << std::endl;
        
        std::cout << "offset = {"
                  << offset.x
                  << ", "
                  << offset.y
                  << "}"
                  << std::endl;
                  
        std::cout << "region = ("
                  << region.origin.x
                  << ", "
                  << region.origin.y
                  << ") -> ["
                  << region.size.width
                  << ", "
                  << region.size.height
                  << "]"
                  << std::endl;
                      
        std::cout << "bounds = ("
                  << bounds.origin.x
                  << ", "
                  << bounds.origin.y
                  << ") -> ["
                  << bounds.size.width
                  << ", "
                  << bounds.size.height
                  << "]"
                  << std::endl;
#endif

        // Region is local to this object.  I.e. region(0,0) is the
        // top left of this object, not the top left of the canvas.
        // Therefore, it's necessary to offset the region by this 
        // component's position.
        munin::rectangle offset_region = region;
        offset_region.origin.x += bounds.origin.x;
        offset_region.origin.y += bounds.origin.y;
        
        // Therefore, the points we want to draw is defined by a rectangle
        // that is the intersection of this component's bounds and
        // the region passed by the canvas.
        boost::optional<munin::rectangle> intersection = 
            munin::intersection(offset_region, bounds);

        if (intersection)
        {
            munin::rectangle &box = *intersection;
            
#ifdef DEBUG_COMPONENT
            std::cout << "intersection = ("
                      << box.origin.x
                      << ", "
                      << box.origin.y
                      << ") -> ["
                      << box.size.width
                      << ", "
                      << box.size.height
                      << "]"
                      << std::endl;
#endif

            for (odin::u32 x = 0; x < box.size.width; ++x)
            {
                for (odin::u32 y = 0; y < box.size.height; ++y)
                {
#ifdef DEBUG_COMPONENT
                    std::cout << "Drawing " << brush_ << " at ("
                              << x + box.origin.x + offset.x
                              << ", "
                              << y + box.origin.y + offset.y
                              << ")" << std::endl;
#endif
                    cvs[ x + box.origin.x + offset.x ]
                       [ y + box.origin.y + offset.y ] = brush_;
                }
            }
        }
    }
};
