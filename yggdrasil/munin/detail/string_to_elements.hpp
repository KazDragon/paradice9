// ==========================================================================
// Munin String-to-elements.
//
// Copyright (C) 2014 Matthew Chaplain, All Rights Reserved.
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
#ifndef YGGDRASIL_MUNIN_DETAIL_STRING_TO_ELEMENTS_HPP_
#define YGGDRASIL_MUNIN_DETAIL_STRING_TO_ELEMENTS_HPP_

#include <string>
#include <vector>

namespace yggdrasil { namespace munin {

class element;

}}

namespace yggdrasil { namespace munin { namespace detail {
    
//* =========================================================================
/// \brief A function that applies the string_to_elements parser to a string.
//* =========================================================================
std::vector<yggdrasil::munin::element> string_to_elements(
    std::string const &text);

//* =========================================================================
/// \brief A function that applies the string_to_elements parser to a string.
//* =========================================================================
std::vector<yggdrasil::munin::element> string_to_elements(
    char const *text);

//* =========================================================================
/// \brief A function that applies the string_to_elements parser to a string.
//* =========================================================================
std::vector<yggdrasil::munin::element> string_to_elements(
    char const *text, size_t len);

}}}

#endif
