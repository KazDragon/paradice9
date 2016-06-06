// ==========================================================================
// Munin View.
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.
//
// Permission to reproduce, distribute, perform, display, and to prepare
// derivitive works from this file under the following conditions:
//
// 1. Any copy, reproduction or derivitive work of any part of this file 
//    contains this copyright notice and licence in its entirety.
//
// 2. The rights granted to you under this license automatically terminate
//    should you attempt to assert any patent claims against the licensor 
//    or contributors, which in any way restrict the ability of any party 
//    from using this software or portions thereof in any form under the
//    terms of this license.
//
// Disclaimer: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
//             KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//             WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//             PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//             OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//             OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//             OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//             SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
// ==========================================================================
#pragma once

#include <munin/basic_container.hpp>
#include <munin/layout.hpp>
#include <boost/any.hpp>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace munin { namespace detail {

// Overload where the next argument is convertible to a component.    
template <class... Args>
void view_helper(
    std::shared_ptr<container> const &content,
    std::shared_ptr<component> const &prev,
    std::shared_ptr<component> const &comp, 
    Args&&... args);

// Overload where the next argument is not convertible to a component.
// In this case, the argument is automatically wrapped in a Boost.Any
// object and passed as the layout hint.
template <
    class Hint, 
    class = typename std::enable_if<
        !std::is_convertible<Hint, std::shared_ptr<component>>::value
    >::type,
    class... Args
>
void view_helper(
    std::shared_ptr<container> const &content,
    std::shared_ptr<component> const &prev,
    Hint&& hint, 
    Args&&... args);

// Overload where there is no next argument.  This is the terminal case.
inline void view_helper(
    std::shared_ptr<container> const &content,
    std::shared_ptr<component> const &prev)
{
    if (prev)
    {
        content->add_component(prev);
    }
}
    
}

//* =========================================================================
/// \brief Constructs a container with a given layout and components.
/// \par Usage
/// Simply call, passing a layout and a series of components and/or
/// layout hints.
/// \par Example #1
/// \code
///     auto v = munin::view(
///         munin::make_compass_layout(),
///         // Note: passing both components and layout hints.
///         munin::make_image("Name: "), munin::COMPASS_LAYOUT_WEST,
///         munin::make_framed_component(
///             munin::make_solid_frame(),
///             munin::make_edit()));
/// \endcode
/// \par Example #2
/// \code
///     auto y = munin::view(
///         munin::make_vertical_squeeze_layout(),
///         // Note: passing components only; no hints.
///         munin::make_button("OK"),
///         munin::make_button("Cancel"));
/// \endcode
//* =========================================================================
template <class... Args>
std::shared_ptr<container> view(
    std::unique_ptr<layout> lyt,
    Args&&... args)
{
    auto comp = std::make_shared<basic_container>();
    comp->set_layout(std::move(lyt));
    detail::view_helper(
        comp, std::shared_ptr<component>{}, std::forward<Args>(args)...);
    
    return comp;
}

namespace detail {
    
template <class... Args>
void view_helper(
    std::shared_ptr<container> const &content, 
    std::shared_ptr<component> const &prev,
    std::shared_ptr<component> const &comp, 
    Args&&... args)
{
    if (prev)
    {
        content->add_component(prev);
    }
    
    view_helper(content, comp, std::forward<Args>(args)...);
}

template <
    class Hint, 
    class,
    class... Args
>
void view_helper(
    std::shared_ptr<container> const &content,
    std::shared_ptr<component> const &prev,
    Hint&& hint, 
    Args&&... args)
{
    if (prev)
    {
        content->add_component(prev, boost::any(std::forward<Hint>(hint)));
    }
    
    view_helper(content, {}, std::forward<Args>(args)...);
}
    
}

}
