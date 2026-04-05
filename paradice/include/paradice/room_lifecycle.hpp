#ifndef PARADICE_ROOM_LIFECYCLE_HPP_
#define PARADICE_ROOM_LIFECYCLE_HPP_

#include "paradice/context.hpp"
#include "paradice/model/room_membership.hpp"

namespace paradice {

inline void disconnect_character(context &context, model::character &character)
{
    auto *room = character.in_room;

    if (room == nullptr)
    {
        return;
    }

    model::leave_room(character);
    context.send_message(*room, character, character.name + " has left Paradice");
}

}  // namespace paradice

#endif
