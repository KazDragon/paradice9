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

#include "odin/filter.hpp"
#include "odin/types.hpp"
#include "odin/telnet/protocol.hpp"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace odin { namespace telnet { 

//* =========================================================================
/// \brief Filters streams of data, a character at a time, for telnet 
/// content. When content is discovered, the appropriate handlers are called.
///
/// \par Usage
/// Instantiate an object of type odin::telnet::filter.  Add handlers to it.
/// For example, one might create an on_command handler thus:
/// \code
///       my_filter_.on_command(boost::bind(&this_class::command, this, _1));
/// \endcode
/// which would mean this_class::command would be called when a command passed
/// through the filter.
/// Then, simply pipe data through the filter.
/// \code
///      odin::telnet::filter my_filter;
/// 
///      my_filter.on_command(boost::bind(&my_class::h, this, _1));
///      std::string string_to_filter = get_string_to_filter();
///      std::string output;
///
///      std::string::iterator iter = string_to_filter.begin();
///      while(iter != string_to_filter.end())
///      {
///          boost::optional<char> ch = my_filter(*iter);
///
///          if(ch)
///          {
///              output += ch;
///          }
///      }
//* =========================================================================
class filter 
    : public odin::filter<odin::u8>
    , boost::noncopyable
{
public :
    typedef boost::function
    <
        void (command)
    > command_callback;
    
    typedef boost::function
    <
        void (negotiation_request, negotiation_type)
    > negotiation_callback;
    
    typedef boost::function
    <
        void (subnegotiation_id_type, subnegotiation_type)
    > subnegotiation_callback;
    
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    filter();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~filter();

    //* =====================================================================
    /// \brief Register a callback for when a telnet command is filtered out.
    //* =====================================================================
    void on_command(command_callback const &callback);
    
    //* =====================================================================
    /// \brief Register a callback for when a telnet negotiation is filtered
    /// out.
    //* =====================================================================
    void on_negotiation(negotiation_callback const &callback);

    //* =====================================================================
    /// \brief Register a callback for when a telnet subnegotiation is
    /// filtered out.
    //* =====================================================================
    void on_subnegotiation(subnegotiation_callback const &callback);
    
private :
    class impl;
    boost::shared_ptr<impl> pimpl_;

    //* =====================================================================
    /// \brief Implemented by the derived class.  Test whether t is
    /// to be accepted or filtered away.
    /// \return true if t can be returned from operator(), false if it is
    /// to be consumed by the filter.
    //* =====================================================================
    virtual bool do_filter(argument_type ch);
};

}}

#endif




