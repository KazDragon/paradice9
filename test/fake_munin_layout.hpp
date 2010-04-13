#include "munin/basic_layout.hpp"

template <class ElementType>
class fake_layout : public munin::basic_layout<ElementType>
{
public :
    fake_layout()
        : do_layout_called(false)
    {
    }
    
    bool do_layout_called;
    
private :
    void do_layout(
        boost::shared_ptr< munin::container<ElementType> > const &container)
    {
        do_layout_called = true;
    }
};
