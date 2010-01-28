#include "paradice/server.hpp"
#include "paradice/socket.hpp"
#include "paradice/client.hpp"
#include "paradice/connection.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/dice_roll_parser.hpp"
#include "paradice/who.hpp"
#include "odin/tokenise.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;
using namespace boost;
using namespace odin;

namespace po = program_options;

asio::io_service io_service;

static string const intro =
"             _                                                         \r\n"
"    |/ _._  | \\.__. _  _ ._/ _                                        \r\n"
"    |\\(_|/_ |_/|(_|(_|(_)| |_>                                        \r\n"
"                    _|                                                 \r\n"
"                                                                       \r\n"
"                   ___                   ___           ___             \r\n"
"                  / _ \\___ ________ ____/ (_)______   / _ \\          \r\n"
"                 / ___/ _ `/ __/ _ `/ _  / / __/ -_)  \\_, /           \r\n"
"                /_/   \\_,_/_/  \\_,_/\\_,_/_/\\__/\\__/  /___/        \r\n"
"                                                           v0.2        \r\n"
"                                                                       \r\n"
"Enter a name by which you wish to be known: ";

static void do_roll(
    string const                 &text, 
    shared_ptr<paradice::client> &client)
{
    static string const usage_message =
        "\r\n Usage:   roll <dice>d<sides>[<bonuses...>]"
        "\r\n Example: roll 2d6+3-20"
        "\r\n\r\n";

    BOOST_AUTO(rolls, paradice::parse_dice_roll(text));

    if (!rolls)
    {
        client->get_connection()->write(usage_message);
        return;
    }

    BOOST_AUTO(dice_roll, rolls.get());

    if (dice_roll.sides_ == 0)
    {
        client->get_connection()->write(usage_message);
        return;
    }

    odin::s32 total = dice_roll.bonus_;
    string roll_description;

    for (odin::u32 current_roll = 0; current_roll < dice_roll.amount_; ++current_roll)
    {
        odin::s32 score = (rand() % dice_roll.sides_) + 1;

        roll_description += str(format("%s%d") 
            % (roll_description.empty() ? "" : ", ")
            % score);

        total += score;
    }

    paradice::message_to_player(
        str(format("\r\nYou roll %dd%d%s%d and score %d [%s]\r\n") 
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % total
            % roll_description)
      , client);

    paradice::message_to_room(
        str(format("\r\n%s rolls %dd%d%s%d and scores %d [%s]\r\n")
            % client->get_name()
            % dice_roll.amount_
            % dice_roll.sides_
            % (dice_roll.bonus_ >= 0 ? "+" : "")
            % dice_roll.bonus_
            % total
            % roll_description)
      , client);
}

static void do_quit(
    string const                       &/*unused*/, 
    shared_ptr<paradice::client> const &client)
{
    client->get_connection()->disconnect();
}

static struct command
{
    string                                                     command_;
    function<void (std::string, shared_ptr<paradice::client>)> function_;
} const command_list[] =
{
    { "!",          NULL                                  }
  , { ".",          bind(&paradice::do_say,       _1, _2) }
  , { "say",        bind(&paradice::do_say,       _1, _2) }

  , { ">",          bind(&paradice::do_sayto,     _1, _2) }
  , { "sayto",      bind(&paradice::do_sayto,     _1, _2) }

  , { ":",          bind(&paradice::do_emote,     _1, _2) }
  , { "emote",      bind(&paradice::do_emote,     _1, _2) }
  
  , { "who",        bind(&paradice::do_who,       _1, _2) }

  , { "set",        bind(&paradice::do_set,       _1, _2) }
  , { "backtrace",  bind(&paradice::do_backtrace, _1, _2) }
  , { "title",      bind(&paradice::do_title,     _1, _2) }
  , { "rename",     bind(&paradice::do_rename,    _1, _2) }
  , { "prefix",     bind(&paradice::do_prefix,    _1, _2) }

  , { "roll",       bind(&do_roll,                _1, _2) }

  , { "quit",       bind(&do_quit,                _1, _2) }
};

void on_name_entered(
    shared_ptr<paradice::client> &client
  , string const                 &input)
{
    pair<string, string> arg = odin::tokenise(input);
    string name = arg.first;

    if (!paradice::is_acceptible_name(name))
    {
        client->get_connection()->write(
            "Your name must be at least three characters long and be composed"
            "of only alphabetical characters.\r\n"
            "Enter a name by which you wish to be known: "); 
    }
    else
    {
        // Ensure correct capitalisation
        name[0] = toupper(name[0]);

        client->set_name(name);

        pair<u16, u16> window_size = 
            client->get_connection()->get_window_size();
            
        string text = str(format(
            "\x1B[2J"      // Erase screen
            "\x1B[9;%dr"   // Set scrolling region
            "\x1B[%d;0f") // Force to end.
            % window_size.second
            % window_size.second);

        client->get_connection()->write(text);

        paradice::message_to_room(
            "\r\n#SERVER: " 
          + client->get_name() 
          + " has entered Paradice.\r\n",
            client);

        client->set_level(paradice::client::level_in_game);

        BOOST_FOREACH(shared_ptr<paradice::client> curr_client, paradice::clients)
        {
            do_who("auto", curr_client);
        }
    }
}

void on_data(
    weak_ptr<paradice::client> &weak_client
  , std::string const          &input);

void on_command(
    shared_ptr<paradice::client> &client
  , std::string const            &input)
{
    pair<string, string> arg = odin::tokenise(input);

    // Transform the command to lower case.
    for (string::iterator ch = arg.first.begin();
         ch != arg.first.end();
         ++ch)
    {
        *ch = tolower(*ch);
    }

    if (arg.first == "!")
    {
        weak_ptr<paradice::client> weak_client(client);
        on_data(weak_client, client->get_last_command());
        return;
    }

    // Search through the list for commands
    BOOST_FOREACH(command const &cur_command, command_list)
    {
        if (cur_command.command_ == arg.first)
        {
            cur_command.function_(arg.second, client);
            client->set_last_command(input);
            return;
        }
    }

    string text = 
        "\r\nDidn't understand that.  Available commands are:\r\n";

    BOOST_FOREACH(command const &cur_command, command_list)
    {
        text += cur_command.command_ + " ";
    }

    text += "\r\n";

    paradice::send_to_player(text, client);
}

void on_data(
    weak_ptr<paradice::client> &weak_client
  , std::string const          &input)
{
    shared_ptr<paradice::client> client = weak_client.lock();

    if (client)
    {
        if (client->get_level() == paradice::client::level_intro_screen)
        {
            on_name_entered(client, input);
        }
        else // client->get_level() == paradice::client::level_in_game
        {
            if (input.empty())
            {
                return;
            }

            if (client->get_command_mode() == paradice::client::command_mode_mud)
            {
                on_command(client, input);
            }
            else
            {
                if (input[0] == '/')
                {
                    if (input.size() >= 2 && input[1] == '!')
                    {
                        on_data(weak_client, "/" + client->get_last_command());
                    }
                    else
                    {
                        on_command(client, input.substr(1));
                    }
                }
                else
                {
                    on_command(client, "say " + input);
                }
            }
        }
    }
}

void on_socket_death(weak_ptr<paradice::client> &weak_client)
{
    shared_ptr<paradice::client> client = weak_client.lock();

    if (client)
    {
        if (client->get_name() != "")
        {
            paradice::message_to_room(
                "\r\n#SERVER: " 
              + client->get_name()
              + " has left Paradice.\r\n",
                client);
        }

        paradice::clients.erase(find(
            paradice::clients.begin()
          , paradice::clients.end(), 
            client));

        BOOST_FOREACH(shared_ptr<paradice::client> curr_client, paradice::clients)
        {
            do_who("auto", curr_client);
        }
    }
}

static void on_window_size_changed(
    weak_ptr<paradice::client> weak_client
  , u16 width
  , u16 height)
{
    shared_ptr<paradice::client> client(weak_client.lock());
    
    if (client)
    {
        if (client->get_level() == paradice::client::level_in_game)
        {
            string text = str(format(
                "\x1B[2J"      // Erase screen
                "\x1B[9;%dr"   // Set scrolling region
                "\x1B[%d;0f")  // Force to end.
              % height
              % height);
            
            client->get_connection()->write(text);

            do_who("auto", client);
            do_backtrace("", client);
        }
    }
}

void on_accept(shared_ptr<paradice::socket> const &socket)
{
    shared_ptr<paradice::client> client(new paradice::client);

    socket->on_death(
        bind(&on_socket_death, weak_ptr<paradice::client>(client)));

    client->set_connection(shared_ptr<paradice::connection>(
        new paradice::connection(
            socket
          , bind(&on_data, weak_ptr<paradice::client>(client), _1))));
    
    paradice::clients.push_back(client);
    
    client->get_connection()->write(intro);
    
    client->get_connection()->on_window_size_changed(
        bind(&on_window_size_changed, 
            weak_ptr<paradice::client>(client),
            _1,
            _2));
}

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned int>(time(NULL)));

    unsigned int port = 4000;
    
    po::options_description description("Available options");
    description.add_options()
        ( "help,h", "show this help message" )
        ( "port,p", po::value<unsigned int>(&port), "port number" )
        ;
        
    po::positional_options_description pos_description;
    pos_description.add("port", -1);
    
    try
    {
        po::variables_map vm;
        po::store(
            po::command_line_parser(argc, argv)
                .options(description)
                .positional(pos_description)
                .run()
          , vm);
        
        po::notify(vm);
        
        if (vm.count("help") != 0)
        {
            throw po::error("");
        }
        else if (vm.count("port") == 0)
        {
            throw po::error("Port number must be specified");
        }
    }
    catch(po::error &err)
    {
        if (strlen(err.what()) == 0)
        {
            cout << format("USAGE: %s <port number>|<options>\n")
                        % argv[0]
                 << description
                 << endl;
                 
            return EXIT_SUCCESS;
        }
        else
        {
            cerr << format("ERROR: %s\n\nUSAGE: %s <port number>|<options>\n")
                        % err.what()
                        % argv[0]
                 << description
                 << endl;
        }
        
        return EXIT_FAILURE;
    }

    paradice::server server(io_service, port, on_accept);
    io_service.run();

    return EXIT_SUCCESS;
}
