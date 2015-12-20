if (GTEST_FOUND)

    set (test_SOURCES
        dice_parser_fixture.cpp
        munin_algorithm_fixture.cpp
    )

    add_executable(paradice_tester ${test_SOURCES})

    target_include_directories(paradice_tester
        PRIVATE
            ${CMAKE_SOURCE_DIR}
    )

    target_compile_features(paradice_tester
        PRIVATE
            cxx_generic_lambdas
    )

    target_link_libraries(
        paradice_tester
        paradice
        munin
        odin
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    add_test(paradice_test paradice_tester)
endif()
