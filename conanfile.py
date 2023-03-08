from conans import ConanFile
from conan.tools.cmake import CMake

class MSGE(ConanFile):
    name = "SenTTBox"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [("psyinf-gmtl/0.7.1")
	            ,("yaml-cpp/0.7.0")
                ,("magic_enum/0.8.1")
                ,("gtest/1.12.1")    
                ,("fmt/9.1.0")
                ,("spdlog/1.11.0")
                ,("nlohmann_json/3.11.2")
                ,("modern-cpp-kafka/2022.12.07")
                ,("entt/3.11.1")
                ,("cereal/1.3.2")
                ]
    generators = "cmake_find_package_multi"
    
    def configure(self):
        self.options['fmt'].header_only = True
        self.options['spdlog'].header_only = True
    def imports(self):    
        self.copy("*.dll", "bin", "bin")
        
