#include "munin/component.hpp"
#include "munin/graphics_context.hpp"
#include "munin/types.hpp"
#include "munin/algorithm.hpp"
#include <algorithm>
#include <boost/weak_ptr.hpp>

//#define DEBUG_COMPONENT

#ifdef DEBUG_COMPONENT
#include <iostream>
#endif

template <class ElementType>
class fake_component : public munin::component<ElementType>
{
public :
    fake_component()
        : brush_()
    {
        bounds_.origin.x    = 0;
        bounds_.origin.y    = 0;
        bounds_.size.width  = 0;
        bounds_.size.height = 0;
        
        preferred_size_.width  = 0;
        preferred_size_.height = 0;
    }
    
    void set_brush(ElementType brush)
    {
        brush_ = brush;
        
        munin::rectangle region;
        region.origin.x = 0;
        region.origin.y = 0;
        region.size = bounds_.size;
        
        std::vector<munin::rectangle> regions;
        regions.push_back(region);
        
        this->on_redraw(regions);
    }
    
    void set_preferred_size(munin::extent const &preferred_size)
    {
        preferred_size_ = preferred_size;
    }
    
private :
    virtual void do_set_position(munin::point const &position)
    {
        bounds_.origin = position;
    }
    
    virtual munin::point do_get_position() const
    {
        return bounds_.origin;
    }
    
    virtual void do_set_size(munin::extent const &size)
    {
        bounds_.size = size;
    }
    
    virtual munin::extent do_get_size() const
    {
        return bounds_.size;
    }
    
    virtual munin::extent do_get_preferred_size() const
    {
        return preferred_size_;
    }
    
    //* =====================================================================
    /// \brief Called by set_parent().  Derived classes must override this
    /// function in order to set the parent of the component in a custom
    /// manner.
    //* =====================================================================
    virtual void do_set_parent(
        boost::shared_ptr< munin::container<ElementType> > const &parent)
    {
        parent_ = parent;
    }
    
    //* =====================================================================
    /// \brief Called by get_parent().  Derived classes must override this
    /// function in order to get the parent of the component in a custom
    /// manner.
    //* =====================================================================
    boost::shared_ptr< munin::container<ElementType> > do_get_parent() const
    {
        return parent_.lock();
    }
    
    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed graphics context.  A component must
    /// only draw the part of itself specified by the region.
    ///
    /// \param context the context in which the component should draw itself.
    /// \param offset the position of the parent component (if there is one)
    ///        relative to the context.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        munin::graphics_context<ElementType> &context
      , munin::point const                   &offset
      , munin::rectangle const               &region)
    {
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
                  << bounds_.origin.x
                  << ", "
                  << bounds_.origin.y
                  << ") -> ["
                  << bounds_.size.width
                  << ", "
                  << bounds_.size.height
                  << "]"
                  << std::endl;
#endif

        // Region is local to this object.  I.e. region(0,0) is the
        // top left of this object, not the top left of the context.
        // Therefore, it's necessary to offset the region by this 
        // component's position.
        munin::rectangle offset_region = region;
        offset_region.origin.x += bounds_.origin.x;
        offset_region.origin.y += bounds_.origin.y;
        
        // Therefore, the points we want to draw is defined by a rectangle
        // that is the intersection of this component's bounds and
        // the region passed by the graphics context.
        boost::optional<munin::rectangle> intersection = 
            munin::intersection(offset_region, bounds_);

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
                    context
                        [ x + box.origin.x + offset.x ]
                        [ y + box.origin.y + offset.y ] = brush_;
                }
            }
        }
    }
    
    boost::weak_ptr< munin::container<ElementType> > parent_;
    munin::rectangle                                 bounds_;
    munin::extent                                    preferred_size_;
    ElementType                                      brush_;
};
