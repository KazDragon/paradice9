set (test_SOURCES
    ansi_parser_fixture.cpp
    paradice_test.cpp
)


enable_testing()

if (CPPUNIT_FOUND)
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
        odin
        cppunit
        ${Boost_SYSTEM_LIBRARY}
    )

    add_test(paradice_test paradice_tester)
endif()

