// ==========================================================================
// Hugin Beast
//
// Copyright (C) 2016 Matthew Chaplain, All Rights Reserved.
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
#include "hugin/model/beast.hpp"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/uuid/random_generator.hpp>

namespace hugin { namespace model {
 
namespace {
    static boost::random_device rdev;
    static boost::mt19937 rng {rdev()};
    static boost::uuids::random_generator uuid_generator(rng);
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast::beast()
  : beast("", "")
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast::beast(std::string const &name, std::string const &description)
  : beast(uuid_generator(), name, description)
{
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast::beast(
    boost::uuids::uuid const &id,
    std::string const &name,
    std::string const &description)
  : id(id),
    name(name),
    description(description)
{
}

// ==========================================================================
// CLONE
// ==========================================================================
beast clone(beast const &source)
{
    return beast(source.name, source.description);
}

}}
