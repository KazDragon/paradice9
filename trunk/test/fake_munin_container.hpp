#include "munin/basic_container.hpp"
#include <vector>
#include <boost/weak_ptr.hpp>

template <class ElementType>
class fake_container 
    : public munin::basic_container<ElementType>
{
public :
    fake_container(ElementType back_brush)
        : back_brush_(back_brush) 
    {
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

    ElementType   back_brush_;
    munin::extent preferred_size_;
};
