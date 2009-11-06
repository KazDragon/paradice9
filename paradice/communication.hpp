#ifndef PARADICE_COMMUNICATION_HPP_
#define PARADICE_COMMUNICATION_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

namespace paradice {

class connection;

// These messages all get backtraced.
void message_to_all(std::string const &text);

void message_to_player(
    std::string const             &text
  , boost::shared_ptr<connection> &conn);

void message_to_room(
    std::string const                   &text
  , boost::shared_ptr<connection> const &conn);

// Sending doesn't backtrace
void send_to_all(std::string const &text);

void send_to_player(
    std::string const             &text
  , boost::shared_ptr<connection> &conn);

void send_to_room(
    std::string const                   &text
  , boost::shared_ptr<connection> const &conn);

}

#endif
