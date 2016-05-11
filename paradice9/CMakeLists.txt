set (paradice9_SOURCES
    src/context_impl.cpp
    src/main.cpp
    src/paradice9.cpp
)

add_executable(paradice9 ${paradice9_SOURCES})

target_include_directories(paradice9
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
#    PRIVATE 
#        ${CMAKE_SOURCE_DIR}
#        ${Boost_INCLUDE_DIRS}
#)

target_compile_definitions(paradice9
    PRIVATE
        BOOST_SIGNALS_NO_DEPRECATION_WARNING
)

target_compile_features(paradice9
    PRIVATE
        cxx_generic_lambdas
)

target_link_libraries(paradice9
    PRIVATE
        paradice
        hugin
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
)
