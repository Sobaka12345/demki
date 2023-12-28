import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class DemkiConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = ["glfw/3.3.8",
                "tinyobjloader/2.0.0-rc10",
                "vulkan-loader/1.3.204.1",
                "glad/0.1.36",
                "glm/0.9.9.8",
				"stb/cci.20220909",
				"pfr/2.1.0"]
    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.presets_prefix = "conan-preset"
        tc.generate()

    def layout(self):
        # We make the assumption that if the compiler is msvc the
        # CMake generator is multi-config
        multi = True if self.settings.get_safe("compiler") == "msvc" else False
        if multi:
            self.folders.generators = os.path.join("generators")
        else:
            self.folders.generators = os.path.join("generators", str(self.settings.build_type))
