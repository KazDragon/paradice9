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
    PRIVATE ${CMAKE_SOURCE_DIR}
)

if (CRYPTO++_FOUND)
    target_compile_definitions(paradice
        PRIVATE
            PARADICE_USE_CRYPTOPP)
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