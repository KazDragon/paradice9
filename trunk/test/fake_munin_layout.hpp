#include "munin/layout.hpp"

template <class ElementType>
class fake_layout : public munin::layout<ElementType>
{
public :
    fake_layout()
        : do_layout_called(false)
    {
        preferred_size_.width  = 0;
        preferred_size_.height = 0;
    }
    
    bool do_layout_called;
    
    void set_preferred_size(munin::extent const &extent)
    {
        preferred_size_ = extent;
    }
    
private :
    void do_layout(
        boost::shared_ptr< munin::container<ElementType> > const &cont)
    {
        do_layout_called = true;
    }
    
    munin::extent do_get_preferred_size(
        boost::shared_ptr< munin::container<ElementType> const> const &cont) const
    {
        return preferred_size_;
    }
    
    munin::extent preferred_size_;
};
