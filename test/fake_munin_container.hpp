#include "munin/basic_container.hpp"
#include <vector>
#include <boost/weak_ptr.hpp>

template <class ElementType>
class fake_container 
    : public munin::basic_container<ElementType>
{
public :
    fake_container()
        : brush_('\0')
    {
    }
    
    void set_brush(ElementType brush)
    {
        brush_ = brush;
    }
    
    void set_preferred_size(munin::extent const &preferred_size)
    {
        preferred_size_ = preferred_size;
    }
    
private :
    virtual munin::extent do_get_preferred_size() const
    {
        return preferred_size_;
    }

    boost::weak_ptr< munin::container<ElementType> > parent_;
    munin::point                                     position_;
    munin::extent                                    size_;    
    munin::extent                                    preferred_size_;
    char                                             brush_;
};
