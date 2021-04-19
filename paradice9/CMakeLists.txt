if (PARADICE9_USE_CONAN)
    set(PARADICE9_PRIVATE_LIBRARIES
        paradice
        CONAN_PKG::boost
        CONAN_PKG::serverpp
        CONAN_PKG::sqlitecpp
)
else()
    find_package(serverpp REQUIRED)
    find_package(SQLiteCpp REQUIRED)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    find_package(gsl-lite 0.34 REQUIRED)
    find_package(Threads REQUIRED)

    set(PARADICE9_PRIVATE_LIBRARIES
        paradice
        KazDragon::serverpp
        SQLiteCpp
        Boost::filesystem
        Boost::boost)
endif()

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

target_compile_features(paradice9
    PRIVATE
        cxx_generic_lambdas
)

target_link_libraries(paradice9
    PRIVATE
        ${PARADICE9_PRIVATE_LIBRARIES}
)
