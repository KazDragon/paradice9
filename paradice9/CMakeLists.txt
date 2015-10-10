set (paradice9_SOURCES
    context_impl.cpp
    main.cpp
    paradice9.cpp
)

add_executable(paradice9 ${paradice9_SOURCES})

target_include_directories(paradice9
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(paradice9
    PRIVATE
        BOOST_SIGNALS_NO_DEPRECATION_WARNING
)

target_compile_features(paradice9
    PRIVATE
        cxx_generic_lambdas
)

target_link_libraries(paradice9
    hugin
    paradice
    munin
    odin
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_RANDOM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_SIGNALS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)
