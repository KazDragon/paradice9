#include "server.hpp"
#include "socket.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "context.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace boost;

namespace paradice
{
    
struct server::impl
{
    impl(asio::io_service                          &io_service
       , weak_ptr<context>                         &context
       , uint16_t                                   port
       , function<void (shared_ptr<socket>)> const &on_accept)
        : io_service_(io_service)
        , context_(context)
        , acceptor_(
            io_service
          , asio::ip::tcp::endpoint(
                asio::ip::tcp::v4()
              , port))
        , keepalive_timer_(io_service)
        , on_accept_(on_accept)
    {
        schedule_accept();
        schedule_keepalive_timer();
    }
       
    void handle_accept(
        shared_ptr<asio::ip::tcp::socket> const &new_socket
      , boost::system::error_code const &error)
    {
        if (!error)
        {
            boost::shared_ptr<socket> paradice_socket(new socket(new_socket));

            on_accept_(paradice_socket);

            schedule_accept();
        }
    }

    void handle_keepalive_timer(
        boost::system::error_code const &error)
    {
        shared_ptr<context> ctx(context_.lock());
        
        if (ctx)
        {
            BOOST_FOREACH(shared_ptr<client> cur_client, ctx->get_clients())
            {
                cur_client->get_connection()->keepalive();
            }
            
            schedule_keepalive_timer();
        }
    }

    void schedule_accept()
    {
        shared_ptr<asio::ip::tcp::socket> new_socket(
            new asio::ip::tcp::socket(io_service_));

        acceptor_.async_accept(
            *new_socket.get()
          , bind(
              &impl::handle_accept
            , this
            , new_socket
            , asio::placeholders::error));
    }
    
    void schedule_keepalive_timer()
    {
        keepalive_timer_.expires_from_now(boost::posix_time::seconds(30));
        keepalive_timer_.async_wait(
            bind(
                &impl::handle_keepalive_timer
              , this
              , asio::placeholders::error));
    }

    asio::io_service                   &io_service_;
    weak_ptr<context>                  &context_;
    asio::ip::tcp::acceptor             acceptor_;
    asio::deadline_timer                keepalive_timer_;
    
    function<void (shared_ptr<socket>)> on_accept_;
};

server::server(
    asio::io_service                          &io_service
  , weak_ptr<context>                         &context
  , uint16_t                                   port
  , function<void (shared_ptr<socket>)> const &on_accept)
    : pimpl_(new impl(io_service, context, port, on_accept))
{
}

server::~server()
{
}

}
