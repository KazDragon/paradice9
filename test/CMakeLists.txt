set (test_SOURCES
    ansi_parser_fixture.cpp
    paradice_test.cpp
    telnet_client_option_fixture.cpp
    telnet_command_router_fixture.cpp
    telnet_negotiation_router_fixture.cpp
    telnet_options_echo_client_fixture.cpp
    telnet_options_naws_client_fixture.cpp
    telnet_options_terminal_type_client_fixture.cpp
    telnet_options_subnegotiationless_client_fixture.cpp
    telnet_parser_fixture.cpp
    telnet_stream_fixture.cpp
    telnet_subnegotiation_router_fixture.cpp
    yggdrasil_munin_attribute_fixture.cpp
    yggdrasil_munin_canvas_fixture.cpp
    yggdrasil_munin_create_model_from_ptree_fixture.cpp
    yggdrasil_munin_detail_string_to_elements_parser_fixture.cpp
    yggdrasil_munin_element_fixture.cpp
    yggdrasil_munin_estring_fixture.cpp
    yggdrasil_munin_extent_fixture.cpp
    yggdrasil_munin_filled_box_fixture.cpp
    yggdrasil_munin_glyph_fixture.cpp
    yggdrasil_munin_image_fixture.cpp
    yggdrasil_munin_point_fixture.cpp
    yggdrasil_munin_rectangle_fixture.cpp
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
    odin
    bragi
    yg_munin
    cppunit
    ${Boost_SYSTEM_LIBRARY}
)

add_test(paradice_test paradice_tester)

