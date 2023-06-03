from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class DemkiConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = ["glfw/3.3.8",
                "vulkan-loader/1.3.231.1",
                "glm/0.9.9.8",
                "stb/cci.20220909"]
    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["MYVAR"] = "MYVAR_VALUE"
        tc.preprocessor_definitions["MYDEFINE"] = "MYDEF_VALUE"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
