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
#include "paradice9/context_impl.hpp"
// #include "paradice/account.hpp"
// #include "paradice/character.hpp"
#include "paradice/client.hpp"
// #include <boost/archive/xml_iarchive.hpp>
// #include <boost/archive/xml_oarchive.hpp>
#include <boost/asio/io_context_strand.hpp>
// #include <boost/filesystem.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
// #include <algorithm>
// #include <fstream>
// #include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include <vector>

// namespace fs = boost::filesystem;

// namespace {
//     static std::shared_ptr<paradice::active_encounter> gm_encounter;
//     static bool gm_encounter_visible = false;
// }

// ==========================================================================
// GET_ACCOUNTS_PATH
// ==========================================================================
// static fs::path get_accounts_path()
// {
//     auto cwd = fs::current_path();
//     auto accounts_path = cwd / "accounts";
    
//     if (!fs::exists(accounts_path))
//     {
//         fs::create_directory(accounts_path);
//     }
    
//     return accounts_path;
// }

// ==========================================================================
// GET_CHARACTERS_PATH
// ==========================================================================
// static fs::path get_characters_path()
// {
//     auto cwd = fs::current_path();
//     auto characters_path = cwd / "characters";
    
//     if (!fs::exists(characters_path))
//     {
//         fs::create_directory(characters_path);
//     }
    
//     return characters_path;
// }

// ==========================================================================
// GET_CHARACTER_ADDRESS
// ==========================================================================
// static std::string get_character_address(
//     std::shared_ptr<paradice::character> const &ch)
// {
//     auto prefix = ch->get_prefix();
//     auto name   = ch->get_name();
//     auto title  = ch->get_suffix();
    
//     std::string address;
    
//     if (!prefix.empty())
//     {
//         address += prefix + " ";
//     }
    
//     address += name;
    
//     if (!title.empty())
//     {
//         address += " " + title;
//     }
    
//     return address;
// }
    
// ==========================================================================
// CONTEXT_IMPL IMPLEMENTATION STRUCTURE
// ==========================================================================
struct context_impl::impl
{
    impl(
        boost::asio::io_context       &io_context,
        boost::filesystem::path const &database_path,
        std::function<void ()>         shutdown)
      : strand_(io_context),
        database_(
            database_path.string(), 
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE),
        shutdown_(std::move(shutdown))
    {
        ensure_schema_created();
    }
    
    // ======================================================================
    // ADD_CLIENT
    // ======================================================================
    void add_client(std::shared_ptr<paradice::client> const &cli)
    {
        strand_.dispatch([this, cli]{clients_.push_back(cli);});
    }

    // ======================================================================
    // REMOVE_CLIENT
    // ======================================================================
    void remove_client(std::shared_ptr<paradice::client> const &cli)
    {
        strand_.dispatch([this, cli]{boost::remove_erase(clients_, cli);});
    }

    // ======================================================================
    // UPDATE_NAMES
    // ======================================================================
    void update_names()
    {
        // std::vector<std::string> names;
        
        // for (auto &cur_client : clients_)
        // {
        //     auto character = cur_client->get_character();
            
        //     if (character != NULL)
        //     {
        //         auto name = get_character_address(character);
                
        //         if (!name.empty())
        //         {
        //             names.push_back(name);
        //         }
        //     }
        // }
        
        for (auto &cur_client : clients_)
        {
            // auto user_interface = cur_client->get_user_interface();
            
            // if (user_interface != NULL)
            // {
            //     user_interface->update_wholist(names);
            // }
        }
    }

    // ======================================================================
    // LOAD_ACCOUNT
    // ======================================================================
    // void load_account(
    //     std::string const &name, std::shared_ptr<paradice::account> &acct)
    // {
    //     auto account_path = get_accounts_path() / name;
        
    //     if (fs::exists(account_path))
    //     {
    //         std::ifstream in(account_path.string().c_str());
    //         boost::archive::xml_iarchive ia(in);
            
    //         acct = std::make_shared<paradice::account>();
    //         ia >> boost::serialization::make_nvp("account", *acct);
    //     }
    // }

    // ======================================================================
    // SAVE_ACCOUNT
    // ======================================================================
    // void save_account(std::shared_ptr<paradice::account> const &acct)
    // {
    //     auto account_path = get_accounts_path() / acct->get_name();
        
    //     std::ofstream out(account_path.string().c_str());
    //     boost::archive::xml_oarchive oa(out);
    //     oa << boost::serialization::make_nvp("account", *acct);
    // }

    // ======================================================================
    // LOAD_CHARACTER
    // ======================================================================
    // void load_character(
    //     std::string const                    &name,
    //     std::shared_ptr<paradice::character> &ch)
    // {
    //     auto character_path = get_characters_path() / name;
        
    //     if (fs::exists(character_path))
    //     {
    //         std::ifstream in(character_path.string().c_str());
    //         boost::archive::xml_iarchive ia(in);
            
    //         ch = std::make_shared<paradice::character>();
    //         ia >> boost::serialization::make_nvp("character", *ch);
    //     }
    // }

    // ======================================================================
    // SAVE_CHARACTER
    // ======================================================================
    // void save_character(std::shared_ptr<paradice::character> const &ch)
    // {
    //     auto character_path = get_characters_path() / ch->get_name();
        
    //     std::ofstream out(character_path.string().c_str());
    //     boost::archive::xml_oarchive oa(out);
    //     oa << boost::serialization::make_nvp("character", *ch);
    // }

    // ======================================================================
    // SHUTDOWN
    // ======================================================================
    void shutdown()
    {
        for (auto &client : clients_)
        {
            client->disconnect();
        }

        shutdown_();
    }

private:
    // ======================================================================
    // ENSURE_ACCOUNT_TABLE_CREATED
    // ======================================================================
    void ensure_accounts_table_created()
    {
        database_.exec(
            "CREATE TABLE IF NOT EXISTS accounts ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    name TEXT UNIQUE,"
            "    password TEXT,"
            "    admin_level INTEGER,"
            "    command_mode INTEGER"
            ");"
        );
    }

    // ======================================================================
    // ENSURE_CHARACTER_TABLE_CREATED
    // ======================================================================
    void ensure_characters_table_created()
    {
        database_.exec(
            "CREATE TABLE IF NOT EXISTS characters ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    name TEXT UNIQUE,"
            "    account_id INTEGER,"
            "    prefix TEXT,"
            "    suffix TEXT,"
            "    gm_level INTEGER,"
            "    FOREIGN KEY (account_id)"
            "        REFERENCES accounts (id)"
            "            ON DELETE CASCADE"
            "            ON UPDATE NO ACTION"
            ");"
        );
    }

    // ======================================================================
    // ENSURE_SCHEMA_CREATED
    // ======================================================================
    void ensure_schema_created()
    {
        database_.exec("PRAGMA foreign_keys=ON;");
        ensure_accounts_table_created();
        ensure_characters_table_created();
    }

    boost::asio::io_context::strand                strand_;
    SQLite::Database                               database_;
    std::function<void ()>                         shutdown_;
    std::vector<std::shared_ptr<paradice::client>> clients_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
context_impl::context_impl(
    boost::asio::io_context       &io_context,
    boost::filesystem::path const &database_path,
    std::function<void ()>         shutdown)
  : pimpl_(new impl(io_context, database_path, std::move(shutdown)))
{
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
// std::vector<std::shared_ptr<paradice::client>> context_impl::get_clients()
// {
//     return pimpl_->clients_;
// }

// ==========================================================================
// ADD_CLIENT
// ==========================================================================
void context_impl::add_client(std::shared_ptr<paradice::client> const &cli)
{
    pimpl_->add_client(cli);
}

// ==========================================================================
// REMOVE_CLIENT
// ==========================================================================
void context_impl::remove_client(std::shared_ptr<paradice::client> const &cli)
{
    pimpl_->remove_client(cli);
}

// ==========================================================================
// UPDATE_NAMES
// ==========================================================================
// void context_impl::update_names()
// {
//     pimpl_->strand_.dispatch([this]{pimpl_->update_names();});
// }

// ==========================================================================
// GET_MONIKER
// ==========================================================================
// std::string context_impl::get_moniker(std::shared_ptr<paradice::character> const &ch)
// {
//     std::string prefix = ch->get_prefix();
//     std::string name   = ch->get_name();
//     std::string title  = ch->get_suffix();
    
//     std::string address;
    
//     if (!prefix.empty())
//     {
//         address += prefix + " ";
//     }
    
//     address += name;
    
//     if (!title.empty())
//     {
//         address += " " + title;
//     }
    
//     return address;
// }
 
// ==========================================================================
// LOAD_ACCOUNT
// ==========================================================================
// std::shared_ptr<paradice::account> context_impl::load_account(std::string const &name)
// {
//     std::shared_ptr<paradice::account> acct;

//     pimpl_->strand_.dispatch([this, &name, &acct]{
//         pimpl_->load_account(name, acct);
//     });

//     return acct;
// }

// ==========================================================================
// SAVE_ACCOUNT
// ==========================================================================
// void context_impl::save_account(std::shared_ptr<paradice::account> const &acct)
// {
//     pimpl_->strand_.dispatch([this, acct]{pimpl_->save_account(acct);});
// }

// ==========================================================================
// LOAD_CHARACTER
// ==========================================================================
// std::shared_ptr<paradice::character> context_impl::load_character(
//     std::string const &name)
// {
//     std::shared_ptr<paradice::character> ch;
//     pimpl_->strand_.dispatch([this, &name, &ch]{
//         pimpl_->load_character(name, ch);
//     });
    
//     return ch;
// }

// ==========================================================================
// SAVE_CHARACTER
// ==========================================================================
// void context_impl::save_character(std::shared_ptr<paradice::character> const &ch)
// {
//     pimpl_->strand_.dispatch([this, ch]{pimpl_->save_character(ch);});
// }

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void context_impl::shutdown()
{
    pimpl_->shutdown();
}

// ==========================================================================
// GET_ACTIVE_ENCOUNTER
// ==========================================================================
// std::shared_ptr<paradice::active_encounter> context_impl::get_active_encounter()
// {
//     if (!gm_encounter) {
//         // gm_encounter = std::make_shared<paradice::active_encounter>();
//     }

//     return gm_encounter;
// }

// ==========================================================================
// SET_ACTIVE_ENCOUNTER
// ==========================================================================
// void context_impl::set_active_encounter(
//     std::shared_ptr<paradice::active_encounter> const &enc)
// {
//     gm_encounter = enc;
// }

// ==========================================================================
// IS_ACTIVE_ENCOUNTER_VISIBLE
// ==========================================================================
// bool context_impl::is_active_encounter_visible() const
// {
//     return gm_encounter_visible;
// }

// ==========================================================================
// SET_ACTIVE_ENCOUNTER_VISIBLE
// ==========================================================================
// void context_impl::set_active_encounter_visible(bool visibility)
// {
//     gm_encounter_visible = visibility;

//     for (auto &cli : pimpl_->clients_)
//     {
//         if (cli)
//         {
//             if (gm_encounter_visible)
//             {
//                 // cli->get_user_interface()->show_active_encounter_window();
//             }
//             else
//             {
//                 // cli->get_user_interface()->hide_active_encounter_window();
//             }
//         }
//     }
// }

// ==========================================================================
// UPDATE_ACTIVE_ENCOUNTER
// ==========================================================================
// void context_impl::update_active_encounter()
// {
//     for (auto &cli : pimpl_->clients_)
//     {
//         if (cli)
//         {
//             // cli->get_user_interface()->set_active_encounter(gm_encounter);
//         }
//     }
// }
