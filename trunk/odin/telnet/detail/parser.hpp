// ==========================================================================
// Odin Telnet Parser.
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
#ifndef ODIN_TELNET_DETAIL_PARSER_HPP_
#define ODIN_TELNET_DETAIL_PARSER_HPP_

#include "odin/lexer.hpp"
#include "odin/types.hpp"
#include "odin/telnet/protocol.hpp"
#include "odin/telnet/detail/adapt_protocol.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace odin { namespace telnet { namespace detail {

class parser
  : public odin::lexer<
        odin::u8
      , boost::variant<
            odin::telnet::command
          , odin::telnet::negotiation_type
          , odin::telnet::subnegotiation_type
          , char
        >
    >
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    parser();

private :
    //* =====================================================================
    /// \brief Query the lexer to see if a complete token is ready.
    /// \return true if the current stored sequence is a complete token,
    /// false otherwise.
    //* =====================================================================
    virtual bool ready() const;

    //* =====================================================================
    /// \brief Clears the stored token.
    //* =====================================================================
    virtual void clear_token();
    
    //* =====================================================================
    /// \brief Store an element
    ///
    /// Stores the passed element in the current sequence. 
    //* =====================================================================
    virtual void store_element(element_type const &element);
    
    //* =====================================================================
    /// \brief Obtain the current stored token
    /// \return the current stored token.
    //* =====================================================================
    virtual token_type const &stored_token() const;

    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}}}

#endif

