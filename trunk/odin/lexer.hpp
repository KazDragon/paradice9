// ==========================================================================
// Odin Lexer.
//
// Copyright (C) 2003 Matthew Chaplain, All Rights Reserved.
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
#ifndef ODIN_LEXER_HPP_
#define ODIN_LEXER_HPP_

#include "odin/interface.hpp"
#include <boost/call_traits.hpp>
#include <boost/concept_check.hpp>

namespace odin {

//* =========================================================================
/// \interface lexer
/// \param Element a singular item to be lexed.
/// \param ElementSequence a sequence of elements that represents a complete
/// token.
/// \brief An interface that models an algorithm that converts lexical elements
/// into tokens.
///
/// Given a set of elements, a lexer is aware of when certain sequences
/// are meaningful (tokens).  For example, a Line Lexer might be passed data.
/// When a \\n is received, the data is meaningful, and can be used.  This 
/// can be detected in two ways.  First and foremost, the ready() method will
/// return true.  token() will also return a default-constructed element 
/// sequence in this case (not a partial sequence - the full sequence is 
/// returned only when the data is ready).
///
/// \par Usage
/// Derive from the class.  Override each pure virtual function so that it
/// reflects the task laid out in the comments below. 
//* =========================================================================
template <typename Element, typename ElementSequence>
class lexer
{
    BOOST_CLASS_REQUIRE(ElementSequence, boost, DefaultConstructibleConcept);
    BOOST_CLASS_REQUIRE(ElementSequence, boost, CopyConstructibleConcept);
    
public :
    //* =====================================================================
    /// \typedef element_type
    /// \brief the type of an element to be lexed.
    //* =====================================================================
    typedef Element         element_type;

    //* =====================================================================
    /// \typedef element_sequence_type
    /// \brief the type of a sequence of elements.
    //* =====================================================================
    typedef ElementSequence element_sequence_type;

    typedef typename boost::call_traits<element_type>::param_type 
        lex_param_type;

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    virtual ~lexer()
    {
    }
       
    //* =====================================================================
    /// \brief Lex an element.
    /// \param element an element to be lexed.
    ///
    /// Adds the element to the current stored sequence.
    //* =====================================================================
    void lex(lex_param_type element)
    {
        if(ready())
        {
            clear_sequence();
        }
        
        store_element(element);
    }

    //* =====================================================================
    /// \brief Return a token
    /// \return If the last element formed a complete sequence, then that
    /// complete sequence is returned.  In all other cases, an empty sequence
    /// is returned.
    //* =====================================================================
    element_sequence_type token() const
    {
        if(ready())
        {
            return stored_sequence();
        }
        else
        {
            return element_sequence_type();
        }
    }

private :
    //* =====================================================================
    /// \brief Query the lexer to see if a complete token is ready.
    /// \return true if the current stored sequence is a complete token,
    /// false otherwise.
    //* =====================================================================
    virtual bool ready() const = 0;

    //* =====================================================================
    /// \brief Clears the stored sequence.
    /// 
    /// Reduces the currently stored sequence to be equivalent to an empty
    /// sequence.
    virtual void clear_sequence() = 0;
    
    //* =====================================================================
    /// \brief Store an element
    ///
    /// Stores the passed element in the current sequence. 
    //* =====================================================================
    virtual void store_element(element_type const &element) = 0;
    
    //* =====================================================================
    /// \brief Obtain the current stored sequence
    /// \return the current stored sequence.
    //* =====================================================================
    virtual element_sequence_type const &stored_sequence() const = 0;
};

}


#endif

