// ==========================================================================
// Paradice Account
//
// Copyright (C) 2011 Matthew Chaplain, All Rights Reserved.
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
#include "paradice/account.hpp"

using namespace odin;
using namespace std;

namespace paradice {
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
account::account()
    : admin_level_(0)
    , command_mode_(command_mode_mud)
{
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
account::~account()
{
}
    
// ==========================================================================
// SET_NAME
// ==========================================================================
void account::set_name(string const &name)
{
    name_ = name;
}

// ==========================================================================
// GET_NAME
// ==========================================================================
string account::get_name() const
{
    return name_;
}

// ==========================================================================
// SET_PASSWORD
// ==========================================================================
void account::set_password(string const &password)
{
    password_ = encrypt(password);
}

// ==========================================================================
// PASSWORD_MATCH
// ==========================================================================
bool account::password_match(string const &password)
{
    return encrypt(password) == password_;
}

// ==========================================================================
// GET_NUMBER_OF_CHARACTERS
// ==========================================================================
u32 account::get_number_of_characters() const
{
    return u32(characters_.size());
}

// ==========================================================================
// GET_CHARACTER_NAME
// ==========================================================================
string account::get_character_name(u32 index)
{
    return characters_[index];
}

// ==========================================================================
// ADD_CHARACTER
// ==========================================================================
void account::add_character(string const &character_name)
{
    characters_.push_back(character_name);
}

// ==========================================================================
// REMOVE_CHARACTER
// ==========================================================================
void account::remove_character(string const &character_name)
{
    characters_.erase(remove(
        characters_.begin(), characters_.end()
      , character_name)
      , characters_.end());
}

// ==========================================================================
// SET_COMMAND_MODE
// ==========================================================================
void account::set_command_mode(command_mode mode)
{
    command_mode_ = mode;
}

// ==========================================================================
// GET_COMMAND_MODE
// ==========================================================================
account::command_mode account::get_command_mode() const
{
    return command_mode_;
}

// ==========================================================================
// GET_ADMIN_LEVEL
// ==========================================================================
u32 account::get_admin_level() const
{
    return admin_level_;
}

}

