#include "communication.hpp"
#include "client.hpp"
#include "connection.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

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

}
