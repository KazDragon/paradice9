#include "communication.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "utility.hpp"
#include "odin/tokenise.hpp"
#include "odin/types.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace odin;

namespace paradice {
    
void message_to_all(string const &text)
{
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        cur_client->get_connection()->write(text);
        cur_client->add_backtrace(
            boost::algorithm::trim_copy(text));
    }
}

void message_to_player(
    string const       &text
  , shared_ptr<client> &conn)
{
    conn->get_connection()->write(text);
    conn->add_backtrace(
        boost::algorithm::trim_copy(text));
}

void message_to_room(
    string const             &text, 
    shared_ptr<client> const &conn)
{
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if (cur_client != conn)
        {
            if (cur_client->get_level() == client::level_in_game)
            {
                cur_client->get_connection()->write(text);
                cur_client->add_backtrace(
                    boost::algorithm::trim_copy(text));
            }
        }
    }
}

void send_to_all(string const &text)
{
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        cur_client->get_connection()->write(text);
    }
}

void send_to_player(
    string const       &text
  , shared_ptr<client> &conn)
{
    conn->get_connection()->write(text);
}

void send_to_room(
    string const             &text, 
    shared_ptr<client> const &conn)
{
    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if (cur_client != conn)
        {
            if (cur_client->get_level() == client::level_in_game)
            {
                cur_client->get_connection()->write(text);
            }
        }
    }
}

void do_say(
    string const       &message, 
    shared_ptr<client> &client)
{
    message_to_player(str(
        format("\r\nYou say, \"%s\"\r\n")
            % message)
      , client);


    message_to_room(str(
        format("\r\n%s says, \"%s\"\r\n")
            % client->get_name()
            % message)
      , client);
}

void do_sayto(
    string const       &message, 
    shared_ptr<client> &player)
{
    pair<string, string> arg = odin::tokenise(message);

    if (arg.first == "")
    {
        send_to_player(
            "\r\n Usage: sayto [player] [message]"
            "\r\n     or"
            "\r\n Usage: > [player] [message]"
            "\r\n"
          , player);

        return;
    }

    BOOST_FOREACH(shared_ptr<client> cur_client, clients)
    {
        if(is_iequal(cur_client->get_name(), arg.first))
        {
            message_to_player(str(
                format("\r\nYou say to %s, \"%s\"\r\n")
                    % cur_client->get_name()
                    % arg.second)
              , player);

            message_to_player(str(
                format("\r\n%s says to you, \"%s\"\r\n")
                    % player->get_name()
                    % arg.second)
              , cur_client);

            return;
        }
    }

    send_to_player(
        "\r\nCouldn't find anyone by that name to talk to.\r\n", player);
}

void do_emote(
    string const       &message
  , shared_ptr<client> &client)
{
    if (message.empty())
    {
        static string const usage_message =
            "\r\n USAGE:   emote <some action>"
            "\r\n EXAMPLE: emote bounces off the walls."
            "\r\n\r\n";

        send_to_player(usage_message, client);
    }

    message_to_all(str(
        format("\r\n%s %s\r\n")
            % client->get_name()
            % message));
}

void do_backtrace(
    string const                 &/*unused*/, 
    shared_ptr<paradice::client> &client)
{
    pair<u16, u16> window_size = client->get_connection()->get_window_size();
    u16 window_width = window_size.first;
    
    string backtrace = "=== Backtrace: ";
    
    if (window_width > u16(backtrace.size()))
    {
        backtrace += string((window_width - 1) - backtrace.size(), '='); 
    }
    
    backtrace = "\r\n" + backtrace + "\r\n";
    
    send_to_player(backtrace + client->get_backtrace(), client);
}


}
