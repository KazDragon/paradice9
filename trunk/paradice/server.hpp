#ifndef PARADICE_SERVER_HPP_
#define PARADICE_SERVER_HPP_

#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace boost { namespace asio {
    class io_service;
}}

namespace paradice {
    
class socket;

class server
{
public :
    server(boost::asio::io_service              &io_service
         , boost::uint16_t                      port
         , boost::function<
               void (boost::shared_ptr<socket>)
            > const                             &on_accept);
        
   ~server();
   
private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif
