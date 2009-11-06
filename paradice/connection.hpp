#ifndef PARADICE_CONNECTION_HPP_
#define PARADICE_CONNECTION_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>

namespace paradice {

class socket;
class client;

class connection
{
public :
    enum level
    {
        level_intro_screen
      , level_in_game
    };
    
    connection();
    ~connection();

    void set_socket(boost::shared_ptr<socket> new_socket);
    void set_client(boost::shared_ptr<client> new_client);
    
    boost::shared_ptr<socket> get_socket();
    boost::shared_ptr<client> get_client();
    
    level get_level() const;
    void set_level(level new_level);
    
    void set_last_command(std::string const &cmd);
    std::string get_last_command() const;

    void add_backtrace(std::string const &text);
    std::string get_backtrace() const;

private :
    struct impl;
    boost::shared_ptr<impl> pimpl_;
};

extern std::vector< boost::shared_ptr<connection> > connections;

}

#endif
