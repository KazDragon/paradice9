// ==========================================================================
// Paradice Beast
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
#include "paradice/beast.hpp"
#include <boost/random.hpp>
#include <boost/uuid/random_generator.hpp>
#include <ctime>

using namespace odin;
using namespace boost::uuids;
using namespace std;

namespace paradice {

namespace {
    boost::mt19937 ran(static_cast<boost::uint32_t>(time(NULL)));
    boost::uuids::random_generator uuid_generator(ran);
}
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
beast::beast()
    : id_(uuid_generator())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
beast::~beast()
{
}

// ==========================================================================
// SET_ID
// ==========================================================================
void beast::set_id(uuid const &id)
{
    id_ = id;
}

// ==========================================================================
// GET_ID
// ==========================================================================
uuid beast::get_id() const
{
    return id_;
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void beast::set_name(string const &name)
{
    name_ = name;
}

// ==========================================================================
// GET_NAME
// ==========================================================================
string beast::get_name() const
{
    return name_;
}

// ==========================================================================
// SET_DESCRIPTION
// ==========================================================================
void beast::set_description(string const &description)
{
    description_ = description;
}

// ==========================================================================
// GET_DESCRIPTION
// ==========================================================================
string beast::get_description() const
{
    return description_;
}

}

