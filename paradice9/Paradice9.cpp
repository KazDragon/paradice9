#include "paradice/server.hpp"
#include "paradice/socket.hpp"
#include "paradice/client.hpp"
#include "paradice/connection.hpp"
#include "paradice/communication.hpp"
#include "paradice/configuration.hpp"
#include "paradice/dice_roll_parser.hpp"
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

bool is_iequal(std::string const &lhs, std::string const &rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    string::const_iterator lch = lhs.begin();
    string::const_iterator rch = rhs.begin();

    while (lch != lhs.end())
    {
        if (toupper(*lch++) != toupper(*rch++))
        {
            return false;
        }
    }

    return true;
}

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

static bool is_acceptible_name(string const &name)
{
    if (name.length() < 3)
    {
        return false;
    }
    
    BOOST_FOREACH(char ch, name)
    {
        if (!is_alpha()(ch))
        {
            return false;
        }
    }
    
    // Profanity filter HERE!
    return true;
}

static void do_say(
    string const                 &message, 
    shared_ptr<paradice::client> &client)
{
    paradice::message_to_player(str(
        format("\r\nYou say, \"%s\"\r\n")
            % message)
      , client);


    paradice::message_to_room(str(
        format("\r\n%s says, \"%s\"\r\n")
            % client->get_name()
            % message)
      , client);
}

static void do_sayto(
    string const                 &message, 
    shared_ptr<paradice::client> &client)
{
    pair<string, string> arg = odin::tokenise(message);

    if (arg.first == "")
    {
        paradice::send_to_player(
            "\r\n Usage: sayto [player] [message]"
            "\r\n     or"
            "\r\n Usage: > [player] [message]"
            "\r\n"
          , client);

        return;
    }

    BOOST_FOREACH(shared_ptr<paradice::client> cur_client, paradice::clients)
    {
        if(::is_iequal(cur_client->get_name(), arg.first))
        {
            paradice::message_to_player(str(
                format("\r\nYou say to %s, \"%s\"\r\n")
                    % cur_client->get_name()
                    % arg.second)
              , cur_client);

            paradice::message_to_player(str(
                format("\r\n%s says to you, \"%s\"\r\n")
                    % cur_client->get_name()
                    % arg.second)
              , cur_client);

            return;
        }
    }

    paradice::send_to_player(
        "\r\nCouldn't find anyone by that name to talk to.\r\n", client);
}

static void do_emote(
    string const                 &message
  , shared_ptr<paradice::client> &client)
{
    if (message.empty())
    {
        static string const usage_message =
            "\r\n USAGE:   emote <some action>"
            "\r\n EXAMPLE: emote bounces off the walls."
            "\r\n\r\n";

        paradice::send_to_player(usage_message, client);
    }

    paradice::message_to_all(str(
        format("\r\n%s %s\r\n")
            % client->get_name()
            % message));
}

static void do_who(
    string const                 &/*unused*/, 
    shared_ptr<paradice::client> &client)
{
    string text =
        "\x1B[s" // save cursor position
        "\x1B[H" // home
        " +=== CURRENTLY PLAYING ======================================================+\r\n";

    BOOST_FOREACH(shared_ptr<paradice::client> cur_client, paradice::clients)
    {
        string address = str(
            format(" | %s %s") 
                % cur_client->get_name()
                % cur_client->get_title());

        size_t width_remaining = 78 - address.size();

        for (; width_remaining != 0; --width_remaining)
        {
            address += " ";
        }

        address += "|\r\n";

        text += address;
    }

    if (paradice::clients.size() <= 6)
    {
        for (unsigned int i = 0; i < (6 - paradice::clients.size()); ++i)
        {
            text += "\x1B[2K |                                                                            |\r\n";
        }
    }
  
    text += " +============================================================================+\r\n";
    text += "\x1B[u"; // unsave

    client->get_connection()->write(text);
}

static void do_title(
    string const                 &title, 
    shared_ptr<paradice::client> &client)
{
    client->set_title(boost::algorithm::trim_copy(title));

    paradice::message_to_player(str(
        format("\r\nYou are now %s %s.\r\n")
            % client->get_name()
            % client->get_title())
      , client);
    
    paradice::message_to_room(str(
        format("\r\n%s is now %s %s.\r\n")
            % client->get_name()
            % client->get_name()
            % client->get_title())
      , client);

    BOOST_FOREACH(shared_ptr<paradice::client> connection, paradice::clients)
    {
        do_who("", connection);
    }
}

static void do_rename(
    string const                 &text,
    shared_ptr<paradice::client> &client)
{
    pair<string, string> name = odin::tokenise(text);
    
    if (!is_acceptible_name(name.first))
    {
        client->get_connection()->write(
            "Your name must be at least three characters long and be composed"
            "of only alphabetical characters.\r\n");
    }
    else
    {
        name.first[0] = toupper(name.first[0]);
        
        string old_name = client->get_name();
        client->set_name(name.first);
        
        client->get_connection()->write(str(
            format("\r\nOk, your name is now %s.\r\n")
                % name.first));
        
        paradice::message_to_room(str(
            format("\r\n%s is now known as %s.\r\n")
                % old_name
                % name.first)
          , client);

        BOOST_FOREACH(shared_ptr<paradice::client> curr_client, paradice::clients)
        {
            do_who("", curr_client);
        }
    }
}

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

static void do_backtrace(
    string const                 &/*unused*/, 
    shared_ptr<paradice::client> &client)
{
    paradice::send_to_player(
        "\r\n==== Backtrace: ==============================================================\r\n"
      + client->get_backtrace()
      , client);
}

static void do_quit(
    string const                       &/*unused*/, 
    shared_ptr<paradice::client> const &client)
{
    shared_ptr<paradice::socket> socket = client->get_socket();

    if (socket)
    {
        socket->kill();
    }
}

static struct command
{
    string                                                     command_;
    function<void (std::string, shared_ptr<paradice::client>)> function_;
} const command_list[] =
{
    { "!",          NULL                            }
  , { ".",          bind(&do_say,           _1, _2) }
  , { "say",        bind(&do_say,           _1, _2) }

  , { ">",          bind(&do_sayto,         _1, _2) }
  , { "sayto",      bind(&do_sayto,         _1, _2) }

  , { ":",          bind(&do_emote,         _1, _2) }
  , { "emote",      bind(&do_emote,         _1, _2) }

  , { "set",        bind(&paradice::do_set, _1, _2) }
  , { "backtrace",  bind(&do_backtrace,     _1, _2) }
  , { "title",      bind(&do_title,         _1, _2) }
  , { "rename",     bind(&do_rename,        _1, _2) }

  , { "roll",       bind(&do_roll,          _1, _2) }

  , { "quit",       bind(&do_quit,          _1, _2) }
};

void on_name_entered(
    shared_ptr<paradice::client> &client
  , string const                 &input)
{
    pair<string, string> arg = odin::tokenise(input);
    string name = arg.first;

    if (!is_acceptible_name(name))
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

        string text = 
            "\x1B[2J"     // Erase screen
            "\x1B[9;24r"  // Set scrolling region
            "\x1B[24;0f"  // force to end.
            ;

        client->get_connection()->write(text);

        paradice::message_to_room(
            "\r\n#SERVER: " 
          + client->get_name() 
          + " has entered Paradice.\r\n",
            client);

        client->set_level(paradice::client::level_in_game);

        BOOST_FOREACH(shared_ptr<paradice::client> curr_client, paradice::clients)
        {
            do_who("", curr_client);
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
        on_data(weak_ptr<paradice::client>(client), client->get_last_command());
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
        paradice::message_to_room(
            "\r\n#SERVER: " 
          + client->get_name()
          + " has left Paradice.\r\n",
            client);

        paradice::clients.erase(find(
            paradice::clients.begin()
          , paradice::clients.end(), 
            client));

        BOOST_FOREACH(shared_ptr<paradice::client> curr_client, paradice::clients)
        {
            do_who("", curr_client);
        }
    }
}

void on_accept(shared_ptr<paradice::socket> const &socket)
{
    shared_ptr<paradice::client> client(new paradice::client);

    socket->on_death(
        bind(&on_socket_death, weak_ptr<paradice::client>(client)));

    client->set_socket(socket);
    client->set_connection(shared_ptr<paradice::connection>(
        new paradice::connection(
            socket
            , bind(&on_data, weak_ptr<paradice::client>(client), _1))));
    
    paradice::clients.push_back(client);
    
    client->get_connection()->write(intro);
}

int main(int argc, char *argv[])
{
    srand((unsigned int)(time(NULL)));

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
