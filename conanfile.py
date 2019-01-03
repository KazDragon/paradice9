from conans import ConanFile, CMake, tools


class Paradice9Conan(ConanFile):
    name = "paradice9"
    version = "2.0.0"
    license = "MIT"
    author = "KazDragon"
    url = "https://github.com/KazDragon/paradice9"
    description = "Telnet Chatter in C++14"
    topics = ("telnet", "mud", "mud-server", "chatter")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    requires = ("boost_algorithm/[>=1.69]@bincrafters/stable",
                "boost_any/[>=1.69]@bincrafters/stable",
                "boost_asio/[>=1.69]@bincrafters/stable",
                "boost_filesystem/[>=1.69]@bincrafters/stable",
                "boost_format/[>=1.69]@bincrafters/stable",
                "boost_optional/[>=1.69]@bincrafters/stable",
                "boost_program_options/[>=1.69]@bincrafters/stable",
                "boost_scope_exit/[>=1.69]@bincrafters/stable",
                "boost_serialization/[>=1.69]@bincrafters/stable",
                "boost_signals2/[>=1.69]@bincrafters/stable",
                "boost_spirit/[>=1.69]@bincrafters/stable",
                "boost_uuid/[>=1.69]@bincrafters/stable",
                "boost_utility/[>=1.69]@bincrafters/stable",
                "boost_variant/[>=1.69]@bincrafters/stable",
                "telnetpp/2.0.0@kazdragon/testing",
                "terminalpp/2.0.0@kazdragon/testing")

#     def source(self):
#         self.run("git clone https://github.com/memsharded/hello.git")
#         self.run("cd hello && git checkout static_shared")
#         # This small hack might be useful to guarantee proper /MT /MD linkage
#         # in MSVC if the packaged project doesn't have variables to set it
#         # properly
#         tools.replace_in_file("hello/CMakeLists.txt", "PROJECT(MyHello)",
#                               '''PROJECT(MyHello)
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()''')

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.hpp", dst="include", src="hello")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("bin/paradice9*", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["paradice9"]

