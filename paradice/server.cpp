#include "server.hpp"
#include "socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

using namespace boost;

namespace paradice
{
    
struct server::impl
{
    impl(asio::io_service                          &io_service
       , uint16_t                                   port
       , function<void (shared_ptr<socket>)> const &on_accept)
        : io_service_(io_service)
        , acceptor_(
            io_service
          , asio::ip::tcp::endpoint(
                asio::ip::tcp::v4()
              , port))
        , on_accept_(on_accept)
    {
        schedule_accept();
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

    asio::io_service                   &io_service_;
    asio::ip::tcp::acceptor             acceptor_;
    
    function<void (shared_ptr<socket>)> on_accept_;
};

server::server(
    asio::io_service                          &io_service
  , uint16_t                                   port
  , function<void (shared_ptr<socket>)> const &on_accept)
    : pimpl_(new impl(io_service, port, on_accept))
{
}

server::~server()
{
}

}
