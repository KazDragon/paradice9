#include "paradice/dice.hpp"

#include <gtest/gtest.h>

TEST(a_dice_scoring_contract, includes_the_roll_bonus_in_the_total)
{
    auto const roll = paradice::dice_roll{
        .repetitions_ = 1,
        .amount_ = 2,
        .sides_ = 6,
        .bonus_ = 3};

    ASSERT_EQ(9, paradice::total_score(roll, {2, 4}));
}

TEST(a_dice_face_generation_contract, requests_a_face_for_each_die_in_the_roll)
{
    auto const roll = paradice::dice_roll{
        .repetitions_ = 1,
        .amount_ = 2,
        .sides_ = 6,
        .bonus_ = 3};

    auto call_count = 0;
    auto const faces = paradice::roll_faces(roll, [&](auto sides) {
        ++call_count;
        EXPECT_EQ(6u, sides);
        return call_count * 2;
    });

    ASSERT_EQ(2, call_count);
    ASSERT_EQ((std::vector<std::int32_t>{2, 4}), faces);
}

TEST(a_dice_roll_reporting_contract, includes_the_total_and_each_die_face)
{
    auto const roll = paradice::dice_roll{
        .repetitions_ = 1,
        .amount_ = 2,
        .sides_ = 6,
        .bonus_ = 3};

    auto const report = paradice::describe_roll_result(roll, {2, 4});

    ASSERT_NE(std::string::npos, report.find("9"));
    ASSERT_NE(std::string::npos, report.find("[2, 4]"));
}
