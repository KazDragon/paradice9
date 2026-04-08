#ifndef PARADICE_MODEL_ROOM_MEMBERSHIP_HPP_
#define PARADICE_MODEL_ROOM_MEMBERSHIP_HPP_

#include "paradice/model/character.hpp"
#include "paradice/model/room.hpp"

namespace paradice::model {

inline void leave_room(character &character)
{
    if (character.in_room == nullptr)
    {
        return;
    }

    auto *room = character.in_room;
    room->characters.remove(&character);
    character.in_room = nullptr;
}

}  // namespace paradice::model

#endif
