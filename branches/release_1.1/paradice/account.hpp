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
#ifndef PARADICE_ACCOUNT_HPP_
#define PARADICE_ACCOUNT_HPP_

#include "odin/types.hpp"
#include "paradice/cryptography.hpp"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/utility.hpp>
#include <vector>
#include <string>

namespace paradice {
    
class account : boost::noncopyable
{
public :
    friend class boost::serialization::access;
    
    //* =====================================================================
    /// \brief Defines the style of command that the client wishes to use.
    //* =====================================================================
    enum command_mode
    {
        command_mode_mud
      , command_mode_mmo
    };
    
    //* =====================================================================
    /// \brief Constructor.
    //* =====================================================================
    account();
    
    //* =====================================================================
    /// \brief Destructor.
    //* =====================================================================
    ~account();
    
    //* =====================================================================
    /// \brief Sets the name of the account.
    //* =====================================================================
    void set_name(std::string const &name);
    
    //* =====================================================================
    /// \brief Retrieves the name of the account.
    //* =====================================================================
    std::string get_name() const;
    
    //* =====================================================================
    /// \brief Sets the password of the account.
    //* =====================================================================
    void set_password(std::string const &password);
    
    //* =====================================================================
    /// \brief Compares the password of the account.
    //* =====================================================================
    bool password_match(std::string const &password);
    
    //* =====================================================================
    /// \brief Retrieves the number of characters belonging to this account.
    //* =====================================================================
    odin::u32 get_number_of_characters() const;
    
    //* =====================================================================
    /// \brief Gets the indexth character name of the account. 
    //* =====================================================================
    std::string get_character_name(odin::u32 index);
    
    //* =====================================================================
    /// \brief Add a character to the account.
    //* =====================================================================
    void add_character(std::string const &character_name);
    
    //* =====================================================================
    /// \brief Remove a character from the account.
    //* =====================================================================
    void remove_character(std::string const &character_name);

    //* =====================================================================
    /// \brief Sets the style of command that the client wishes to use.
    //* =====================================================================
    void set_command_mode(command_mode mode);

    //* =====================================================================
    /// \brief Returns the style of command that the client is using.
    //* =====================================================================
    command_mode get_command_mode() const;
    
    //* =====================================================================
    /// \brief Returns the admin level of the account.
    //* =====================================================================
    odin::u32 get_admin_level() const;
    
    //* =====================================================================
    /// \brief Serializes an account to or from an archive.
    //* =====================================================================
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(name_);
        ar & BOOST_SERIALIZATION_NVP(password_);
        ar & BOOST_SERIALIZATION_NVP(command_mode_);
        ar & BOOST_SERIALIZATION_NVP(characters_);

        // In version 1, passwords were unencrypted.  Therefore, we must
        // encrypt it in order for it to work properly.
        if (version == 1)
        {
            password_ = encrypt(password_);
        }

        // Before version 2, there were no admin levels.
        if (version < 2)
        {
            admin_level_ = 0;
        }
        else
        {
            ar & BOOST_SERIALIZATION_NVP(admin_level_);
        }
    }
    
private :
    std::string              name_;
    std::string              password_;
    odin::u32                admin_level_;
    command_mode             command_mode_;
    std::vector<std::string> characters_;
};

}

BOOST_CLASS_VERSION(paradice::account, 2)

#endif

