// ==========================================================================
// Odin Telnet Filter.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_TELNET_FILTER_HPP_
#define ODIN_TELNET_FILTER_HPP_

/*
#include "odin/filter.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>

namespace odin { namespace telnet { 

// FILTER ====================================================================
//  FUNCTION : Filters streams of data, a character at a time, for telnet 
//             content.  When content is discovered, the appropriate handlers 
//             are called.
//
//  USAGE    : Instantiate an object of type telnet_filter.  Add handlers to 
//             it.  For example, one might create an on_command handler thus:
//
//       my_filter_.on_command(boost::bind(&this_class::command, this, _1));
//
//             which would mean this_class::command would be called when a
//             command passed through the filter.
//
//             Then, simply pipe data through the filter.
//
//  EXAMPLE  :
//
//       odin::net::telnet_filter my_filter;
// 
//       my_filter.on_command(boost::bind(&my_class::h, this, _1));
//       std::string string_to_filter = get_string_to_filter();
//       std::string output;
//
//       std::string::iterator iter = string_to_filter.begin();
//       while(iter != string_to_filter.end())
//       {
//           boost::optional<char> ch = my_filter(*iter);
//
//           if(ch)
//           {
//               output += ch;
//           }
//       }
//   
// ===========================================================================
class filter : public odin::filter<char>
{
    // =======================================================================
    // PUBLIC TYPEDEFS 
    // =======================================================================
    public :
        typedef boost::function
        <
            void (command)
        > command_callback;
        
        typedef boost::function
        <
            void (negotiation_type, negotiation_request)
        > negotiation_callback;
        
        typedef boost::function
        <
            void (subnegotiation_type, subnegotiation, unsigned int)
        > subnegotiation_callback;
        
    // =======================================================================
    // CONSTRUCTOR 
    // =======================================================================
    filter();

    // =======================================================================
    // DESTRUCTOR 
    // =======================================================================
    virtual ~filter();

    // =======================================================================
    // PUBLIC MEMBER FUNCTIONS 
    // =======================================================================
    public :
        void on_command(command_callback const &callback);
        void on_negotiation(negotiation_callback const &callback);
        void on_subnegotiation(subnegotiation_callback const &callback);
        
    // =======================================================================
    // PRIVATE IMPLEMENTATION
    // =======================================================================
    private :
        struct impl;
        impl *pimpl_;

    // =======================================================================
    // PRIVATE VIRTUAL MEMBER FUNCTIONS 
    // =======================================================================
    private :
        virtual bool do_parse(argument_type ch);
};

}}
*/

#endif




