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
#ifndef PARADICE_BEAST_HPP_
#define PARADICE_BEAST_HPP_

#include "odin/types.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/utility.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <string>

namespace paradice {
    
class beast : boost::noncopyable
{
public :
    friend class boost::serialization::access;
    
    //* =====================================================================
    /// \brief Constructor.
    //* =====================================================================
    beast();
    
    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~beast();
    
    //* =====================================================================
    /// \brief Sets the ID of the beast.
    //* =====================================================================
    void set_id(boost::uuids::uuid const &id);

    //* =====================================================================
    /// \brief Retreives the ID of the beast.
    //* =====================================================================
    boost::uuids::uuid get_id() const;

    //* =====================================================================
    /// \brief Sets the name of the beast.
    //* =====================================================================
    void set_name(std::string const &name);
    
    //* =====================================================================
    /// \brief Retrieves the name of the beast.
    //* =====================================================================
    std::string get_name() const;
    
    //* =====================================================================
    /// \brief Sets the description of the beast.
    //* =====================================================================
    void set_description(std::string const &description);

    //* =====================================================================
    /// \brief Retrieves the description of the beast.
    //* =====================================================================
    std::string get_description() const;

    //* =====================================================================
    /// \brief Serializes an character to or from an archive.
    //* =====================================================================
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(id_);
        ar & BOOST_SERIALIZATION_NVP(name_);
        ar & BOOST_SERIALIZATION_NVP(description_);
    }
    
private :
    boost::uuids::uuid id_;
    std::string        name_;
    std::string        description_;
};

}

BOOST_CLASS_VERSION(paradice::beast, 1)

#endif

