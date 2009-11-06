#include "connection.hpp"
#include <boost/foreach.hpp>
#include <deque>
#include <string>
#include <vector>

using namespace boost;
using namespace std;

namespace paradice {

vector< shared_ptr<connection> > connections;
    
struct connection::impl
{
    shared_ptr<socket> socket_;
    shared_ptr<client> client_;
    connection::level  level_;

    string             last_command_;
    deque<string>      backtrace_;
};

connection::connection()
    : pimpl_(new impl)
{
    pimpl_->level_  = level_intro_screen;
}
    
connection::~connection()
{
}

void connection::set_socket(shared_ptr<socket> new_socket)
{
    pimpl_->socket_ = new_socket;
}

void connection::set_client(shared_ptr<client> new_client)
{
    pimpl_->client_ = new_client;
}

shared_ptr<socket> connection::get_socket()
{
    return pimpl_->socket_;
}

shared_ptr<client> connection::get_client()
{
    return pimpl_->client_;
}

connection::level connection::get_level() const
{
    return pimpl_->level_;
}

void connection::set_level(level new_level)
{
    pimpl_->level_ = new_level;
}

void connection::set_last_command(string const &cmd)
{
    pimpl_->last_command_ = cmd;
}

string connection::get_last_command() const
{
    return pimpl_->last_command_;
}

void connection::add_backtrace(string const &text)
{
    pimpl_->backtrace_.push_back(text);

    if (pimpl_->backtrace_.size() > 10)
    {
        pimpl_->backtrace_.pop_front();
    }
}

string connection::get_backtrace() const
{
    string text;

    BOOST_FOREACH(string trace, pimpl_->backtrace_)
    {
        text += trace + "\r\n";
    }

    return text;
}

}
