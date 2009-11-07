#ifndef PARADICE_COMMUNICATION_HPP_
#define PARADICE_COMMUNICATION_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

namespace paradice {

class client;

// These messages all get backtraced.
void message_to_all(std::string const &text);

void message_to_player(
    std::string const         &text
  , boost::shared_ptr<client> &conn);

void message_to_room(
    std::string const               &text
  , boost::shared_ptr<client> const &conn);

// Sending doesn't backtrace
void send_to_all(std::string const &text);

void send_to_player(
    std::string const         &text
  , boost::shared_ptr<client> &conn);

void send_to_room(
    std::string const               &text
  , boost::shared_ptr<client> const &conn);

}

#endif
