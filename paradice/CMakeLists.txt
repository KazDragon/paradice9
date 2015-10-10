set (paradice_SOURCES
    account.cpp
    active_encounter.cpp
    admin.cpp
    beast.cpp
    character.cpp
    client.cpp
    communication.cpp
    configuration.cpp
    connection.cpp
    cryptography.cpp
    dice_roll_parser.cpp
    encounter.cpp
    gm.cpp
    help.cpp
    random.cpp
    rules.cpp
    utility.cpp
    who.cpp
)

add_library(paradice
    STATIC ${paradice_SOURCES})

target_include_directories(paradice
    PRIVATE 
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/telnetpp/include
)

# Select the best cryptography available.
if (CRYPTO++_FOUND)
    target_compile_definitions(paradice
        PRIVATE
            PARADICE_USE_CRYPTOPP)
            
    target_link_libraries(paradice
        LINK_INTERFACE_LIBRARIES
            cryptopp)
else()
    target_compile_definitions(paradice
        PRIVATE
            PARADICE_NOCRYPT)
endif()

target_compile_definitions(paradice
    PRIVATE
        BOOST_SIGNALS_NO_DEPRECATION_WARNING
)

target_compile_features(paradice
    PRIVATE
        cxx_right_angle_brackets
        cxx_generic_lambdas
)

target_link_libraries(paradice
    LINK_INTERFACE_LIBRARIES
        telnetpp
)

