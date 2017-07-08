set (PARADICE9_SOURCE_FILES
    src/context_impl.cpp
    src/main.cpp
    src/paradice9.cpp
)

set (PARADICE9_INCLUDE_FILES
    include/paradice9/context_impl.hpp
    include/paradice9/paradice9.hpp
)

add_executable(paradice9
    ${PARADICE9_SOURCE_FILES}
    ${PARADICE9_INCLUDE_FILES}
)

target_include_directories(paradice9
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
    PRIVATE
        paradice
        hugin
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
)
