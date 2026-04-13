find_package(GTest CONFIG REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(cryptopp REQUIRED)

if (NOT TARGET KazDragon::munin)
    find_package(munin 0.7.1 CONFIG REQUIRED)
endif()

add_executable(paradice_ui_tester
    admin_command_fixture.cpp
    command_catalog_fixture.cpp
    client_roster_fixture.cpp
    dice_fixture.cpp
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
        KazDragon::munin
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

add_test(NAME paradice_ui_test COMMAND paradice_ui_tester)

add_executable(paradice9_system_tester
    context_impl_fixture.cpp
    system_fixture.cpp
    ../paradice9/src/context_impl.cpp
    ../paradice9/src/cryptography.cpp
)

target_include_directories(paradice9_system_tester
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../paradice/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../paradice9/include
)

target_link_libraries(paradice9_system_tester
    PRIVATE
        paradice
        KazDragon::munin
        SQLiteCpp
        cryptopp::cryptopp
        Boost::filesystem
        Boost::boost
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

add_test(NAME paradice9_system_test COMMAND paradice9_system_tester)
