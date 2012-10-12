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

    fake_canvas()
        : columns_(0)
        , rows_(0)
    {
    }
        
private :
    virtual void set_value(
        odin::s32    column
      , odin::s32    row
      , element_type value)
    {
        values_[column + columns_][row + rows_] = value;
    }

    virtual element_type get_value(
        odin::s32 column
      , odin::s32 row) const
    {
        return const_cast<fake_canvas&>(*this).values_
            [column + columns_][row + rows_];
    }
    
    virtual void do_apply_offset(
        odin::s32 columns
      , odin::s32 rows)
    {
        columns_ += columns;
        rows_ += rows;
    }
    
    odin::s32 columns_;
    odin::s32 rows_;
};
