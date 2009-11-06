#include "paradice/server.hpp"
#include "paradice/socket.hpp"
#include "paradice/client.hpp"
#include "paradice/connection.hpp"
#include "paradice/communication.hpp"
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

pair<string, string> tokenise(string const &text)
{
    BOOST_AUTO(space_pos, find(text.begin(), text.end(), ' '));

    return make_pair(
        boost::algorithm::trim_copy(string(text.begin(), space_pos))
      , boost::algorithm::trim_copy(string(space_pos, text.end())));
}

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
    string const                     &message, 
    shared_ptr<paradice::connection> &conn)
{
    paradice::message_to_player(str(
        format("\r\nYou say, \"%s\"\r\n")
            % message)
      , conn);


    paradice::message_to_room(str(
        format("\r\n%s says, \"%s\"\r\n")
            % conn->get_client()->get_name()
            % message)
      , conn);
}

static void do_sayto(
    string const                     &message, 
    shared_ptr<paradice::connection> &conn)
{
    pair<string, string> arg = tokenise(message);

    if (arg.first == "")
    {
        paradice::send_to_player(
            "\r\n Usage: sayto [player] [message]"
            "\r\n     or"
            "\r\n Usage: > [player] [message]"
            "\r\n"
          , conn);

        return;
    }

    BOOST_FOREACH(shared_ptr<paradice::connection> cur_conn, paradice::connections)
    {
        if(::is_iequal(cur_conn->get_client()->get_name(), arg.first))
        {
            paradice::message_to_player(str(
                format("\r\nYou say to %s, \"%s\"\r\n")
                    % cur_conn->get_client()->get_name()
                    % arg.second)
              , conn);

            paradice::message_to_player(str(
                format("\r\n%s says to you, \"%s\"\r\n")
                    % conn->get_client()->get_name()
                    % arg.second)
              , cur_conn);

            return;
        }
    }

    paradice::send_to_player(
        "\r\nCouldn't find anyone by that name to talk to.\r\n", conn);
}

static void do_who(
    string const                           &/*unused*/, 
    shared_ptr<paradice::connection> const &conn)
{
    string text =
        "\x1B[s" // save cursor position
        "\x1B[H" // home
        " +=== CURRENTLY PLAYING ======================================================+\r\n";

    BOOST_FOREACH(shared_ptr<paradice::connection> cur_conn, paradice::connections)
    {
        string address = str(
            format(" | %s %s") 
                % cur_conn->get_client()->get_name()
                % cur_conn->get_client()->get_title());

        size_t width_remaining = 78 - address.size();

        for (; width_remaining != 0; --width_remaining)
        {
            address += " ";
        }

        address += "|\r\n";

        text += address;
    }

    if (paradice::connections.size() <= 6)
    {
        for (unsigned int i = 0; i < (6 - paradice::connections.size()); ++i)
        {
            text += "\x1B[2K |                                                                            |\r\n";
        }
    }
  
    text += " +============================================================================+\r\n";
    text += "\x1B[u"; // unsave

    conn->get_client()->write(text);
}

static void do_title(
    string const                     &title, 
    shared_ptr<paradice::connection> &conn)
{
    conn->get_client()->set_title(boost::algorithm::trim_copy(title));

    paradice::message_to_player(str(
        format("\r\nYou are now %s %s.\r\n")
            % conn->get_client()->get_name()
            % conn->get_client()->get_title())
      , conn);
    
    paradice::message_to_room(str(
        format("\r\n%s is now %s %s.\r\n")
            % conn->get_client()->get_name()
            % conn->get_client()->get_name()
            % conn->get_client()->get_title())
      , conn);

    BOOST_FOREACH(shared_ptr<paradice::connection> connection, paradice::connections)
    {
        do_who("", connection);
    }
}

static void do_rename(
    string const                           &text,
    shared_ptr<paradice::connection> const &conn)
{
    pair<string, string> name = tokenise(text);
    
    if (!is_acceptible_name(name.first))
    {
        conn->get_client()->write(
            "Your name must be at least three characters long and be composed"
            "of only alphabetical characters.\r\n");
    }
    else
    {
        name.first[0] = toupper(name.first[0]);
        
        string old_name = conn->get_client()->get_name();
        conn->get_client()->set_name(name.first);
        
        conn->get_client()->write(str(
            format("\r\nOk, your name is now %s.\r\n")
                % name.first));
        
        paradice::message_to_room(str(
            format("\r\n%s is now known as %s.\r\n")
                % old_name
                % name.first)
          , conn);

        BOOST_FOREACH(shared_ptr<paradice::connection> connection, paradice::connections)
        {
            do_who("", connection);
        }
    }
}

static void do_roll(
    string const                     &text, 
    shared_ptr<paradice::connection> &conn)
{
    static string const usage_message =
        "\r\n Usage:   roll [number of dice] [number of sides] [opt:bonuses...]"
        "\r\n Example: roll 2 6 +3 -20"
        "\r\n";

    pair<string, string> roll = tokenise(text);

    if (roll.first.empty() || roll.second.empty())
    {
        conn->get_client()->write(usage_message);
        return;
    }

    int bonus_score = 0;

    pair<string, string> bonus = tokenise(roll.second);
    bonus = tokenise(bonus.second);

    while (bonus.first != "")
    {
        bonus_score += atoi(bonus.first.c_str());
        bonus = tokenise(bonus.second);
    }
    
    int rolls = atoi(roll.first.c_str());
    int sides = atoi(roll.second.c_str());

    if (rolls == 0 || sides == 0 || rolls > 100)
    {
        conn->get_client()->write(usage_message);
        return;
    }

    int total = bonus_score;

    string roll_description;

    for (int current_roll = 0; current_roll < rolls; ++current_roll)
    {
        int score = (rand() % sides) + 1;

        roll_description += str(format("%s%d") 
            % (roll_description.empty() ? "" : ", ")
            % score);

        total += score;
    }

    paradice::message_to_player(
        str(format("\r\nYou roll %dd%d%s%d and score %d [%s]\r\n") 
            % rolls
            % sides
            % (bonus_score >= 0 ? "+" : "")
            % bonus_score
            % total
            % roll_description)
      , conn);

    paradice::message_to_room(
        str(format("\r\n%s rolls %dd%d%s%d and scores %d [%s]\r\n")
            % conn->get_client()->get_name()
            % rolls
            % sides
            % (bonus_score >= 0 ? "+" : "")
            % bonus_score
            % total
            % roll_description)
      , conn);
}

static void do_backtrace(
    string const                     &/*unused*/, 
    shared_ptr<paradice::connection> &conn)
{
    paradice::send_to_player(
        "\r\n==== Backtrace: ==============================================================\r\n"
      + conn->get_backtrace()
      , conn);
}

static void do_quit(
    string const                           &/*unused*/, 
    shared_ptr<paradice::connection> const &conn)
{
    shared_ptr<paradice::socket> socket = conn->get_socket();

    if (socket)
    {
        socket->kill();
    }
}

static struct command
{
    string                                                         command_;
    function<void (std::string, shared_ptr<paradice::connection>)> function_;
} const command_list[] =
{
    { "!",          NULL                            }
  , { ".",          bind(&do_say,           _1, _2) }
  , { "say",        bind(&do_say,           _1, _2) }

  , { ">",          bind(&do_sayto,         _1, _2) }
  , { "sayto",      bind(&do_sayto,         _1, _2) }

//, { "who",        bind(&do_who,           _1, _2) }
  , { "backtrace",  bind(&do_backtrace,     _1, _2) }
  , { "title",      bind(&do_title,         _1, _2) }
  , { "rename",     bind(&do_rename,        _1, _2) }

  , { "roll",       bind(&do_roll,          _1, _2) }

  , { "quit",       bind(&do_quit,          _1, _2) }
};

void on_name_entered(
    shared_ptr<paradice::connection> &conn
  , string const                     &input)
{
    pair<string, string> arg = tokenise(input);
    string name = arg.first;

    if (!is_acceptible_name(name))
    {
        conn->get_client()->write(
            "Your name must be at least three characters long and be composed"
            "of only alphabetical characters.\r\n"
            "Enter a name by which you wish to be known: "); 
    }
    else
    {
        // Ensure correct capitalisation
        name[0] = toupper(name[0]);

        conn->get_client()->set_name(name);

        string text = 
            "\x1B[2J"     // Erase screen
            "\x1B[9;24r"  // Set scrolling region
            "\x1B[24;0f"  // force to end.
            ;

        conn->get_client()->write(text);

        paradice::message_to_room(
            "\r\n#SERVER: " 
          + conn->get_client()->get_name() 
          + " has entered Paradice.\r\n",
            conn);

        conn->set_level(paradice::connection::level_in_game);

        BOOST_FOREACH(shared_ptr<paradice::connection> connection, paradice::connections)
        {
            do_who("", connection);
        }
    }
}

void on_data(
    weak_ptr<paradice::connection> &weak_connection
  , std::string const              &input)
{
    shared_ptr<paradice::connection> conn = weak_connection.lock();

    if (conn)
    {
        if (conn->get_level() == paradice::connection::level_intro_screen)
        {
            on_name_entered(conn, input);
        }
        else // conn->get_level() == paradice::connection::level_in_game
        {
            pair<string, string> arg = tokenise(input);

            // Transform the command to lower case.
            for (string::iterator ch = arg.first.begin();
                 ch != arg.first.end();
                 ++ch)
            {
                *ch = tolower(*ch);
            }

            if (arg.first == "!")
            {
                on_data(weak_connection, conn->get_last_command());
                return;
            }

            // Search through the list for commands
            BOOST_FOREACH(command const &cur_command, command_list)
            {
                if (cur_command.command_ == arg.first)
                {
                    cur_command.function_(arg.second, conn);
                    conn->set_last_command(input);
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

            paradice::send_to_player(text, conn);
        }
    }
}

void on_socket_death(weak_ptr<paradice::connection> &weak_connection)
{
    shared_ptr<paradice::connection> conn = weak_connection.lock();

    if (conn)
    {
        paradice::message_to_room(
            "\r\n#SERVER: " 
          + conn->get_client()->get_name()
          + " has left Paradice.\r\n",
            conn);

        paradice::connections.erase(find(
            paradice::connections.begin()
          , paradice::connections.end(), 
            conn));

        BOOST_FOREACH(shared_ptr<paradice::connection> connection, paradice::connections)
        {
            do_who("", connection);
        }
    }
}

void on_accept(shared_ptr<paradice::socket> const &socket)
{
    shared_ptr<paradice::connection> connection(new paradice::connection);
    
    socket->on_death(
        bind(&on_socket_death, weak_ptr<paradice::connection>(connection)));
    
    connection->set_socket(socket);
    connection->set_client(shared_ptr<paradice::client>(new paradice::client(
        socket
      , bind(&on_data, weak_ptr<paradice::connection>(connection), _1))));
    
    paradice::connections.push_back(connection);
    
    connection->get_client()->write(intro);
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
