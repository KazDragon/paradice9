// ==========================================================================
// Paradice Rules
//
// Copyright (C) 2009 Matthew Chaplain, All Rights Reserved.  
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef PARADICE_RULES_HPP_
#define PARADICE_RULES_HPP_

#include "command.hpp"
#include "odin/core.hpp"
#include <memory>
#include <string>

namespace paradice {

class client;

struct roll_data
{
    std::weak_ptr<client> roller;
    std::string           name;
    std::string           roll_text;
    odin::s32             raw_score;
    odin::s32             score;
    bool                  max_roll;
};

PARADICE_COMMAND_DECL(roll);
PARADICE_COMMAND_DECL(rollprivate);
PARADICE_COMMAND_DECL(showrolls);
PARADICE_COMMAND_DECL(clearrolls);

}

#endif
