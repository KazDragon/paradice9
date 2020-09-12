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
    requires = ("boost/[>=1.69]",
                "cryptopp/[>=8.2.0]@bincrafters/stable",
                "sqlitecpp/2.4.0",
                "munin/[>=0.3.7]@kazdragon/conan-public",
                "telnetpp/[>=2.0.1]@kazdragon/conan-public",
                "terminalpp/[>=1.4.0]@kazdragon/conan-public",
                "serverpp/[>=0.0.3]@kazdragon/conan-public")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

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

