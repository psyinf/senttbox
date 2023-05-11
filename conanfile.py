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
                ,("imgui/cci.20230105+1.89.2.docking")
                ,("vulkan-headers/1.3.239.0")
                ,("vulkan-loader/1.3.239.0")
                ,("glfw/3.3.2")
                ,("glew/2.1.0")
                ,("vsg/1.0.3")
                ,("glslang/11.5.0")
                ,("sdl/2.26.1")
                ]
    generators = "cmake_find_package_multi"
    
    def configure(self):
        self.options['fmt'].header_only = True
        self.options['spdlog'].header_only = True
        self.options['vsg'].shared = False
        self.options['vsg'].shader_compiler = True
        #self.options['imgui'].shared = True
        #self.options['vulkan-loader'].shared = True
    def imports(self):    
        self.copy("*.dll", "bin", "bin")
        self.copy("imgui_impl_glfw.*", dst="bindings", src="./res/bindings")
        self.copy("imgui_impl_vulkan.*", dst="bindings", src="./res/bindings")
        self.copy("imgui_impl_open*", dst="bindings", src="./res/bindings")
        self.copy("imgui_impl_sdl.*", dst="bindings", src="./res/bindings")
     