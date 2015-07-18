// ==========================================================================
// Odin Router.
//
// Copyright (C) 2008 Matthew Chaplain, All Rights Reserved.
// This file is covered by the MIT Licence:
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// ==========================================================================
#ifndef ODIN_ROUTER_HPP_
#define ODIN_ROUTER_HPP_

#include <functional>
#include <map>
#include <utility>

namespace odin {

//* =========================================================================
/// \class router
/// \brief A class template to route messages.
/// \param Key A token that identifies the handler for a message.  It must
/// be derivable from a message in some way.
/// \param Message A message that this router receives and must forward
/// appropriately.
/// \param KeyFromMessagePolicy A template class that implements the
/// following function: \code
/// static void KeyFromMessagePolicy::key_from_message(Message const &message)
/// \endcode This function should return a Key that is derived from the Message.
///
/// A class template that receives messages and forwards them on to
/// appropriate handlers.
///
/// \par Usage
/// Create an instance of the class, passing the necessary template parameters.
/// For this demonstration, we will use a char as a Key, a string as a Message,
/// and a hypothetical class first_element<> that returns the first
/// element of the string as its key.
/// \par
/// \code router<char, std::string, first_element> route; \endcode
/// \par
/// We want to forward any strings beginning with the letter 'z' to a
/// hypothetical function, "z_handler".
/// \par
/// \code route.register_route('z', z_handler); \endcode
/// \par
/// Anything else should go to a hypothetical function "unwanted_handler".
/// \par
/// \code route.set_unregistered_route(unwanted_handler); \endcode
/// \par
/// Given a vector<std::string> called "vec", we want to route all of the
/// strings therein.
/// \par
/// \code std::for_each(vec.begin(), vec.end(), route); \endcode
//* =========================================================================
template <
    class Key
  , class Message
  , class KeyFromMessagePolicy
>
class router
{
public :
    typedef Key                                  key_type;
    typedef Message                              message_type;
    typedef std::function<void (Message const&)> function_type;
    typedef std::map
    <
        key_type
      , function_type
    > registered_functions_map_type;

    //* =====================================================================
    /// \brief Register a route for messages with a particular key.
    ///
    /// Register a route that will later on be taken by any message received
    /// that contains the specified key.
    //* =====================================================================
    template <typename KeyType, typename Function>
    void register_route(
        KeyType  &&key,
        Function &&function)
    {
        registered_functions_[std::forward<KeyType>(key)] =
            std::forward<Function>(function);
    }

    //* =====================================================================
    /// \brief Unregisters a route for messages with a particular key.
    ///
    /// Unregister a route that will no longer be taken by any message
    /// received that contains the specified key.
    //* =====================================================================
    template <typename KeyType>
    void unregister_route(KeyType &&key)
    {
        registered_functions_.erase(std::forward<KeyType>(key));
    }

    //* =====================================================================
    /// \brief Set a route for messages with an unregistered key.
    ///
    /// Register a route that will be taken by any message received that has
    /// a key for which a route has not been registered.
    //* =====================================================================
    template <typename Function>
    void set_unregistered_route(Function &&route)
    {
        unregistered_route_ = std::forward<Function>(route);
    }

    //* =====================================================================
    /// \brief Route a message.
    ///
    /// Routes a message.  If the key for the message has a registered route,
    /// then it will be forwarded to that handler.  Otherwise, if there is
    /// an unregistered route, then the message will be forwarded to that
    /// handler instead.
    //* =====================================================================
    void operator()(Message const& message) const
    {
        auto iter = registered_functions_.find(
            KeyFromMessagePolicy::key_from_message(message));

        if (iter != registered_functions_.end())
        {
            iter->second(message);
        }
        else if (unregistered_route_ != NULL)
        {
            unregistered_route_(message);
        }
    }

private :
    registered_functions_map_type registered_functions_;
    function_type                 unregistered_route_;
};

}

#endif
