#include <gtest/gtest.h>

#include <paradice/model/character.hpp>
#include <paradice/model/room.hpp>
#include <paradice/model/room_membership.hpp>

TEST(a_character, can_leave_their_current_room)
{
    paradice::model::room room;
    paradice::model::character character{};

    character.in_room = &room;
    room.characters.push_back(&character);

    paradice::model::leave_room(character);

    ASSERT_EQ(nullptr, character.in_room);
    ASSERT_TRUE(room.characters.empty());
}
