#include "munin/canvas.hpp"
#include "odin/types.hpp"
#include <map>

template <class ElementType>
struct fake_canvas : munin::canvas<ElementType>
{
    typedef typename munin::canvas<
        ElementType
    >::element_type element_type;
    
    std::map< odin::s32, std::map<odin::s32, ElementType> > values_;

private :
    virtual void set_value(
        odin::s32    column
      , odin::s32    row
      , element_type value)
    {
        values_[column][row] = value;
    }

    virtual element_type get_value(
        odin::s32 column
      , odin::s32 row) const
    {
        return const_cast<fake_canvas&>(*this).values_[column][row];
    }
};
