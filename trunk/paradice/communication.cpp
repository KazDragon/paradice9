#include "communication.hpp"
#include "connection.hpp"
#include "client.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace paradice {
    
void message_to_all(string const &text)
{
    BOOST_FOREACH(shared_ptr<connection> cur_conn, connections)
    {
        cur_conn->get_client()->write(text);
        cur_conn->add_backtrace(
            boost::algorithm::trim_copy(text));
    }
}

void message_to_player(
    string const           &text
  , shared_ptr<connection> &conn)
{
    conn->get_client()->write(text);
    conn->add_backtrace(
        boost::algorithm::trim_copy(text));
}

void message_to_room(
    string const                 &text, 
    shared_ptr<connection> const &conn)
{
    BOOST_FOREACH(shared_ptr<connection> cur_conn, connections)
    {
        if (cur_conn != conn)
        {
            if (cur_conn->get_level() == connection::level_in_game)
            {
                cur_conn->get_client()->write(text);
                cur_conn->add_backtrace(
                    boost::algorithm::trim_copy(text));
            }
        }
    }
}

void send_to_all(string const &text)
{
    BOOST_FOREACH(shared_ptr<connection> cur_conn, connections)
    {
        cur_conn->get_client()->write(text);
    }
}

void send_to_player(
    string const           &text
  , shared_ptr<connection> &conn)
{
    conn->get_client()->write(text);
}

void send_to_room(
    string const                 &text, 
    shared_ptr<connection> const &conn)
{
    BOOST_FOREACH(shared_ptr<connection> cur_conn, connections)
    {
        if (cur_conn != conn)
        {
            if (cur_conn->get_level() == connection::level_in_game)
            {
                cur_conn->get_client()->write(text);
            }
        }
    }
}

}
