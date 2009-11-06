#ifndef PARADICE_INFORMATION_HPP_
#define PARADICE_INFORMATION_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

namespace paradice {

class connection;

void send_to_all(std::string const &text);

void send_to_room(
    std::string const                   &text
  , boost::shared_ptr<connection> const &conn);

}

#endif
