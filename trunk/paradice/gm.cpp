// ==========================================================================
// Paradice Encounters
//
// Copyright (C) 2013 Matthew Chaplain, All Rights Reserved.  
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
#include "paradice/gm.hpp"
#include "paradice/client.hpp"
#include "paradice/communication.hpp"
#include "hugin/user_interface.hpp"
#include "odin/tokenise.hpp"
#include <boost/typeof/typeof.hpp>

using namespace odin;
using namespace std;

namespace paradice {

// ==========================================================================
// PARADICE COMMAND: GM
// ==========================================================================
PARADICE_COMMAND_IMPL(gm)
{
    string argument = tokenise(arguments).first;
    
    BOOST_AUTO(user_interface, player->get_user_interface());
    
    if (argument == "tools")
    {
        user_interface->select_face(hugin::FACE_GM_TOOLS);
        user_interface->set_focus();
    }
    else if (argument == "show")
    {
        // pop up encounters for all.
    }
    else
    {
        send_to_player(
            ctx
          , "<TODO: usage statement>\n"
          , player);
    }

}

}
