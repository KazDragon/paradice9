#include "munin/graphics_context.hpp"
#include "odin/types.hpp"
#include <map>

template <class ElementType>
struct fake_graphics_context : munin::graphics_context<ElementType>
{
    typedef typename munin::graphics_context<
        ElementType
    >::element_type element_type;
    
    std::map< odin::u32, std::map<odin::u32, ElementType> > values_;

private :
    virtual void set_value(
        odin::u32    row
      , odin::u32    column
      , element_type value)
    {
        values_[row][column] = value;
    }

    virtual element_type get_value(
        odin::u32 row
      , odin::u32 column) const
    {
        return const_cast<fake_graphics_context&>(*this).values_[row][column];
    }
};
