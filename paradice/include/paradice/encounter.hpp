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
#ifndef PARADICE_ENCOUNTER_HPP_
#define PARADICE_ENCOUNTER_HPP_

#include "paradice/export.hpp"
#include "paradice/beast.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <string>
#include <memory>
#include <vector>

namespace paradice {

class PARADICE_EXPORT encounter : boost::noncopyable
{
public :
    friend class boost::serialization::access;

    //* =====================================================================
    /// \brief Constructor.
    //* =====================================================================
    encounter();

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~encounter();

    //* =====================================================================
    /// \brief Sets the ID of the Encounter.
    //* =====================================================================
    void set_id(boost::uuids::uuid const &id);

    //* =====================================================================
    /// \brief Retreives the ID of the Encounter.
    //* =====================================================================
    boost::uuids::uuid get_id() const;

    //* =====================================================================
    /// \brief Sets the name of the Encounter.
    //* =====================================================================
    void set_name(std::string const &name);

    //* =====================================================================
    /// \brief Retrieves the name of the Encounter.
    //* =====================================================================
    std::string get_name() const;

    //* =====================================================================
    /// \brief Sets the beasts of the Encounter.
    //* =====================================================================
    void set_beasts(std::vector<std::shared_ptr<paradice::beast>> beasts);

    //* =====================================================================
    /// \brief Retrieves the beasts of the Encounter.
    //* =====================================================================
    std::vector<std::shared_ptr<paradice::beast>> get_beasts() const;

    //* =====================================================================
    /// \brief Serializes an character to or from an archive.
    //* =====================================================================
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(id_);
        ar & BOOST_SERIALIZATION_NVP(name_);
        ar & BOOST_SERIALIZATION_NVP(beasts_);
    }

private :
    boost::uuids::uuid                            id_;
    std::string                                   name_;
    std::vector<std::shared_ptr<paradice::beast>> beasts_;
};

}

BOOST_CLASS_VERSION(paradice::encounter, 1)

#endif

