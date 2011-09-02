// ==========================================================================
// Paradice Context Implementation
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
#include "context_impl.hpp"
#include "paradice/account.hpp"
#include "paradice/character.hpp"
#include "paradice/client.hpp"
#include "hugin/user_interface.hpp"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace boost;
using namespace boost::archive;
using namespace boost::filesystem;
using namespace odin;

// ==========================================================================
// GET_ACCOUNTS_PATH
// ==========================================================================
static path get_accounts_path()
{
    BOOST_AUTO(cwd, current_path());
    BOOST_AUTO(accounts_path, cwd / "accounts");
    
    if (!exists(accounts_path))
    {
        create_directory(accounts_path);
    }
    
    return accounts_path;
}

// ==========================================================================
// GET_CHARACTERS_PATH
// ==========================================================================
static path get_characters_path()
{
    BOOST_AUTO(cwd, current_path());
    BOOST_AUTO(characters_path, cwd / "characters");
    
    if (!exists(characters_path))
    {
        create_directory(characters_path);
    }
    
    return characters_path;
}

// ==========================================================================
// GET_CHARACTER_ADDRESS
// ==========================================================================
static string get_character_address(shared_ptr<paradice::character> &ch)
{
    string prefix = ch->get_prefix();
    string name   = ch->get_name();
    string title  = ch->get_suffix();
    
    string address;
    
    if (!prefix.empty())
    {
        address += prefix + " ";
    }
    
    address += name;
    
    if (!title.empty())
    {
        address += " " + title;
    }
    
    return address;
}
    
// ==========================================================================
// CONTEXT_IMPL IMPLEMENTATION STRUCTURE
// ==========================================================================
struct context_impl::impl
{
    shared_ptr<odin::net::server>          server_;
    vector< shared_ptr<paradice::client> > clients_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
context_impl::context_impl(shared_ptr<odin::net::server> server)
    : pimpl_(new impl)
{
    pimpl_->server_ = server;
}
    
// ==========================================================================
// DESTRUCTOR
// ==========================================================================
context_impl::~context_impl()
{
}
    
// ==========================================================================
// GET_CLIENTS
// ==========================================================================
vector< shared_ptr<paradice::client> > context_impl::get_clients()
{
    return pimpl_->clients_;
}

// ==========================================================================
// ADD_CLIENT
// ==========================================================================
void context_impl::add_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->clients_.push_back(cli);
}

// ==========================================================================
// REMOVE_CLIENT
// ==========================================================================
void context_impl::remove_client(shared_ptr<paradice::client> const &cli)
{
    pimpl_->clients_.erase(
        std::remove(
            pimpl_->clients_.begin()
          , pimpl_->clients_.end()
          , cli)
      , pimpl_->clients_.end());
}

// ==========================================================================
// UPDATE_NAMES
// ==========================================================================
void context_impl::update_names()
{
    vector<string> names;
    
    BOOST_FOREACH(shared_ptr<paradice::client> cur_client, pimpl_->clients_)
    {
        BOOST_AUTO(character, cur_client->get_character());
        
        if (character != NULL)
        {
            BOOST_AUTO(name, get_character_address(character));
            
            if (!name.empty())
            {
                names.push_back(name);
            }
        }
    }
    
    BOOST_FOREACH(shared_ptr<paradice::client> cur_client, pimpl_->clients_)
    {
        BOOST_AUTO(user_interface, cur_client->get_user_interface());
        
        if (user_interface != NULL)
        {
            user_interface->update_wholist(names);
        }
    }
}

// ==========================================================================
// GET_MONIKER
// ==========================================================================
string context_impl::get_moniker(shared_ptr<paradice::character> &ch)
{
    string prefix = ch->get_prefix();
    string name   = ch->get_name();
    string title  = ch->get_suffix();
    
    string address;
    
    if (!prefix.empty())
    {
        address += prefix + " ";
    }
    
    address += name;
    
    if (!title.empty())
    {
        address += " " + title;
    }
    
    return address;
}
 
// ==========================================================================
// LOAD_ACCOUNT
// ==========================================================================
shared_ptr<paradice::account> context_impl::load_account(string const &name)
{
    BOOST_AUTO(
        account_path
      , get_accounts_path() / name);
    
    if (exists(account_path))
    {
        try
        {
            ifstream in(account_path.string().c_str());
            xml_iarchive ia(in);
            
            shared_ptr<paradice::account> account(new paradice::account);
            ia >> boost::serialization::make_nvp("account", *account);
            
            return account;
        }
        catch (std::exception &ex)
        {
            cerr << "Exception caught trying to load account: "
                 << account_path.string()
                 << "\n"
                 << "    Error: "
                 << ex.what()
                 << endl;

            return shared_ptr<paradice::account>();
        }
    }
    
    return shared_ptr<paradice::account>();
}

// ==========================================================================
// SAVE_ACCOUNT
// ==========================================================================
void context_impl::save_account(shared_ptr<paradice::account> acct)
{
    BOOST_AUTO(
        account_path
      , get_accounts_path() / acct->get_name());
    
    try
    {
        ofstream out(account_path.string().c_str());
        xml_oarchive oa(out);
        oa << boost::serialization::make_nvp("account", *acct);
    }
    catch (std::exception &ex)
    {
        cerr << "Exception caught trying to save account: "
             << account_path.string()
             << "\n"
             << "    Error: "
             << ex.what()
             << endl;
    }
}

// ==========================================================================
// LOAD_CHARACTER
// ==========================================================================
shared_ptr<paradice::character> context_impl::load_character(
    string const &name)
{
    BOOST_AUTO(
        character_path
      , get_characters_path() / name);
    
    if (exists(character_path))
    {
        try
        {
            ifstream in(character_path.string().c_str());
            xml_iarchive ia(in);
            
            shared_ptr<paradice::character> character(new paradice::character);
            ia >> boost::serialization::make_nvp("character", *character);
            
            return character;
        }
        catch (std::exception &ex)
        {
            cerr << "Exception caught trying to load character: "
                 << character_path.string()
                 << "\n"
                 << "    Error: "
                 << ex.what()
                 << endl;

            return shared_ptr<paradice::character>();
        }
    }
    
    return shared_ptr<paradice::character>();
}

// ==========================================================================
// SAVE_CHARACTER
// ==========================================================================
void context_impl::save_character(shared_ptr<paradice::character> ch)
{
    BOOST_AUTO(
        character_path
      , get_characters_path() / ch->get_name());
    
    try
    {
        ofstream out(character_path.string().c_str());
        xml_oarchive oa(out);
        oa << boost::serialization::make_nvp("character", *ch);
    }
    catch (std::exception &ex)
    {
        cerr << "Exception caught trying to save character: "
             << character_path.string()
             << "\n"
             << "    Error: "
             << ex.what()
             << endl;
    }
}

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void context_impl::shutdown()
{
    pimpl_->server_->shutdown();
}

