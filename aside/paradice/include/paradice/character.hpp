// ==========================================================================
// Paradice Character
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
#ifndef PARADICE_CHARACTER_HPP_
#define PARADICE_CHARACTER_HPP_

#include "paradice/export.hpp"
#include "paradice/beast.hpp"
#include "paradice/encounter.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/version.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

namespace paradice {

class PARADICE_EXPORT character : boost::noncopyable
{
public :
    friend class boost::serialization::access;

    //* =====================================================================
    /// \brief Constructor.
    //* =====================================================================
    character();

    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~character();

    //* =====================================================================
    /// \brief Sets the name of the character.
    //* =====================================================================
    void set_name(std::string const &name);

    //* =====================================================================
    /// \brief Retrieves the name of the character.
    //* =====================================================================
    std::string get_name() const;

    //* =====================================================================
    /// \brief Sets the character's name prefix.
    //* =====================================================================
    void set_prefix(std::string const &prefix);

    //* =====================================================================
    /// \brief Retrieves the character's name prefix.
    //* =====================================================================
    std::string get_prefix() const;

    //* =====================================================================
    /// \brief Sets the character's name suffix.
    //* =====================================================================
    void set_suffix(std::string const &suffix);

    //* =====================================================================
    /// \brief Retrieves the character's name suffix.
    //* =====================================================================
    std::string get_suffix() const;

    //* =====================================================================
    /// \brief Sets the character's GM level.
    //* =====================================================================
    void set_gm_level(std::uint32_t level);

    //* =====================================================================
    /// \brief Retrieves the character's GM level.
    //* =====================================================================
    std::uint32_t get_gm_level() const;

    //* =====================================================================
    /// \brief Sets the character's beasts.
    //* =====================================================================
    void set_beasts(std::vector<std::shared_ptr<beast>> const &beasts);

    //* =====================================================================
    /// \brief Retrieves the character's beasts.
    //* =====================================================================
    std::vector<std::shared_ptr<beast>> get_beasts() const;

    //* =====================================================================
    /// \brief Sets the character's encounters.
    //* =====================================================================
    void set_encounters(
        std::vector<std::shared_ptr<encounter>> const &encounters);

    //* =====================================================================
    /// \brief Retreives the character's encounters.
    //* =====================================================================
    std::vector<std::shared_ptr<encounter>> get_encounters() const;

    //* =====================================================================
    /// \brief Serializes an character to or from an archive.
    //* =====================================================================
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(name_);
        ar & BOOST_SERIALIZATION_NVP(prefix_);
        ar & BOOST_SERIALIZATION_NVP(suffix_);

        // In version 2, the concept of a GM level was introduced.
        if (version >= 2)
        {
            ar & BOOST_SERIALIZATION_NVP(gm_level_);
        }
        else
        {
            gm_level_ = 0;
        }

        /* TODO: serialization to std::shared_ptr<> need some work.
        // In version 3, the concept of a bestiary was introduced.
        if (version >= 3)
        {
            ar & BOOST_SERIALIZATION_NVP(beasts_);
        }

        // In version 4, the concept of encounters was introduced.
        if (version >= 4)
        {
            ar & BOOST_SERIALIZATION_NVP(encounters_);
        }
        */
    }

private :
    std::string                             name_;
    std::string                             prefix_;
    std::string                             suffix_;
    std::uint32_t                               gm_level_;
    std::vector<std::shared_ptr<beast>>     beasts_;
    std::vector<std::shared_ptr<encounter>> encounters_;
};

}

BOOST_CLASS_VERSION(paradice::character, 4)

#endif

