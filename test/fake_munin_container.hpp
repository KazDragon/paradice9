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

    void do_initialise_region(
        munin::canvas<ElementType> &cvs
      , munin::point const         &offset
      , munin::rectangle const     &region)
    {
        for (odin::s32 row = 0; row < region.size.width; ++row)
        {
            for (odin::s32 column = 0; column < region.size.height; ++column)
            {
                cvs[offset.x + region.origin.x + row   ]
                   [offset.y + region.origin.y + column] = back_brush_;
            }
        }
    }

    ElementType   back_brush_;
    munin::extent preferred_size_;
};
