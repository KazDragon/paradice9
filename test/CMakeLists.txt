find_package(GTest CONFIG REQUIRED)

add_executable(paradice_ui_tester
    client_roster_fixture.cpp
    dice_roll_parser_fixture.cpp
    room_membership_fixture.cpp
    roster_fixture.cpp
)

target_include_directories(paradice_ui_tester
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../paradice/include
)

target_link_libraries(paradice_ui_tester
    PRIVATE
        paradice
        munin
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

add_test(NAME paradice_ui_test COMMAND paradice_ui_tester)
