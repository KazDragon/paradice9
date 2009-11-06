#ifndef PARADICE_CLIENT_HPP_
#define PARADICE_CLIENT_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <string>

namespace paradice {

class socket;

class client
{
public :
    client(
        boost::shared_ptr<socket>           client_socket
      , boost::function<void (std::string)> data_callback);
    
    ~client();
    
    void set_name(std::string const &name);
    std::string get_name() const;
    
    void set_title(std::string const &title);
    std::string get_title() const;
    
    void write(std::string const &text);

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

}

#endif
