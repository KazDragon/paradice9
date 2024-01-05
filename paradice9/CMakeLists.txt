find_package(serverpp 0.2.0 REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(cryptopp REQUIRED)
find_package(gsl-lite 0.38 REQUIRED)
find_package(Threads REQUIRED)

set (PARADICE9_SOURCE_FILES
    src/context_impl.cpp
    src/cryptography.cpp
    src/main.cpp
    src/server.cpp
)

set (PARADICE9_INCLUDE_FILES
    include/paradice9/context_impl.hpp
    include/paradice9/cryptography.hpp
    include/paradice9/server.hpp
)

add_executable(paradice9
    ${PARADICE9_SOURCE_FILES}
    ${PARADICE9_INCLUDE_FILES}
)

target_include_directories(paradice9
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(paradice9
    PRIVATE
        cxx_generic_lambdas
)

target_link_libraries(paradice9
    PRIVATE
        paradice
        KazDragon::serverpp
        SQLiteCpp
        cryptopp::cryptopp
        Boost::filesystem
        Boost::boost
)
