#include "munin/container.hpp"
#include <vector>
#include <boost/weak_ptr.hpp>

template <class ElementType>
class fake_container : public munin::container<ElementType>
{
public :
    fake_container()
        : brush_('\0')
    {
        position_.x  = 0;
        position_.y  = 0;
        size_.width  = 0;
        size_.height = 0;
    }
    
    void set_brush(ElementType brush)
    {
        brush_ = brush;
    }
    
private :
    virtual odin::u32 do_get_number_of_components() const
    {
        return odin::u32(components_.size());
    }
    
    virtual void do_add_component(
        boost::shared_ptr< munin::component<ElementType> > const &component)
    {
        components_.push_back(component);
    }
    
    virtual boost::shared_ptr< munin::component<ElementType> > do_get_component(
        odin::u32 index) const
    {
        return components_[index];
    }
    
    virtual void do_set_position(munin::point const &position)
    {
        position_ = position;
    }
    
    virtual munin::point do_get_position() const
    {
        return position_;
    }
    
    virtual void do_set_size(munin::extent const &size)
    {
        size_ = size;
    }
    
    virtual munin::extent do_get_size() const
    {
        return size_;
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


    boost::weak_ptr< munin::container<ElementType> > parent_;     
    munin::point                                     position_;
    munin::extent                                    size_;         
    char                                             brush_;
    std::vector< 
        boost::shared_ptr< munin::component<ElementType> >
    > components_;
};
