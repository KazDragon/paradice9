// ==========================================================================
// Paradice Encounter
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
#include "paradice/encounter.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <ctime>

namespace paradice {

namespace {
    boost::uuids::random_generator uuid_generator;
}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
encounter::encounter()
    : id_(uuid_generator())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
encounter::~encounter()
{
}

// ==========================================================================
// SET_ID
// ==========================================================================
void encounter::set_id(boost::uuids::uuid const &id)
{
    id_ = id;
}

// ==========================================================================
// GET_ID
// ==========================================================================
boost::uuids::uuid encounter::get_id() const
{
    return id_;
}

// ==========================================================================
// SET_NAME
// ==========================================================================
void encounter::set_name(std::string const &name)
{
    name_ = name;
}

// ==========================================================================
// GET_NAME
// ==========================================================================
std::string encounter::get_name() const
{
    return name_;
}

// ==========================================================================
// SET_BEASTS
// ==========================================================================
void encounter::set_beasts(std::vector<std::shared_ptr<beast>> beasts)
{
    beasts_ = beasts;
}

// ==========================================================================
// GET_BEASTS
// ==========================================================================
std::vector<std::shared_ptr<beast>> encounter::get_beasts() const
{
    return beasts_;
}

}

