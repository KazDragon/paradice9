// ==========================================================================
// Munin Image Component.
//
// Copyright (C) 2010 Matthew Chaplain, All Rights Reserved.
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
#ifndef MUNIN_IMAGE_HPP_
#define MUNIN_IMAGE_HPP_

#include "munin/basic_component.hpp"
#include <vector>

namespace terminalpp {
    class string;
}

namespace munin {

//* =========================================================================
/// \brief A class that models a single-line text control with a frame
/// bordering it.
//* =========================================================================
class MUNIN_EXPORT image : public munin::basic_component
{
public :
    //* =====================================================================
    /// \brief Constructor
    /// \param elements - a multidimentional array with each element
    /// representing one horizontal line of the image.
    //* =====================================================================
    image(std::vector<terminalpp::string> const &elements);

    //* =====================================================================
    /// \brief Constructor
    /// \param elements - an array representing a single-lined image.
    //* =====================================================================
    image(terminalpp::string const &elements);

    //* =====================================================================
    /// \brief Sets the image displayed
    //* =====================================================================
    void set_image(std::vector<terminalpp::string> const &elements);

    //* =====================================================================
    /// \brief Sets the image displayed
    //* =====================================================================
    void set_image(terminalpp::string const &elements);

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~image();

protected :
    //* =====================================================================
    /// \brief Called by get_preferred_size().  Derived classes must override
    /// this function in order to get the size of the component in a custom
    /// manner.
    //* =====================================================================
    virtual terminalpp::extent do_get_preferred_size() const;

    //* =====================================================================
    /// \brief Called by draw().  Derived classes must override this function
    /// in order to draw onto the passed context.  A component must only draw
    /// the part of itself specified by the region.
    ///
    /// \param ctx the context in which the component should draw itself.
    /// \param region the region relative to this component's origin that
    /// should be drawn.
    //* =====================================================================
    virtual void do_draw(
        context         &ctx
      , rectangle const &region);

private :
    struct impl;
    std::shared_ptr<impl> pimpl_;
};

//* =========================================================================
/// \brief Returns a newly created image
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<image> make_image(
    std::vector<terminalpp::string> const &elements);

//* =========================================================================
/// \brief Returns a newly created image
//* =========================================================================
MUNIN_EXPORT
std::shared_ptr<image> make_image(terminalpp::string const &elements);

}

#endif

