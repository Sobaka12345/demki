from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get
import os

required_conan_version = ">=2.0"


class SPIRVReflectConan(ConanFile):
    name = "spirv-reflect"
    description = "SPIRV-Reflect is a lightweight library that provides a C/C++ reflection API for SPIR-V shader bytecode in Vulkan applications."
    homepage = "https://github.com/KhronosGroup/SPIRV-Reflect"
    topics = ("spirv", "spirv-v", "vulkan", "opengl", "opencl", "khronos")
    license = "Apache License, Version 2.0"
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"

    options = {
        "shared": [True, False],
    }

    default_options = {
        "shared": False,
    }

    short_paths = True

    def layout(self):
        layout_folder = f"SPIRV-Reflect-vulkan-sdk-{self.version}"
        cmake_layout(self, src_folder=layout_folder, build_folder=layout_folder)

    def requirements(self):
        self.requires(f'vulkan-loader/{self.version}')

    def source(self):
        package_name = "sdk" if self.version < "1.3.268.0" else "vulkan-sdk"

        get(self, f'https://github.com/KhronosGroup/SPIRV-Reflect/archive/refs/tags/{package_name}-{self.version}.tar.gz', strip_root=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["SPIRV_REFLECT_EXAMPLES"] = False
        tc.cache_variables["SPIRV_REFLECT_EXECUTABLE"] = False
        tc.cache_variables["SPIRV_REFLECT_STATIC_LIB"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="spirv_reflect.h*", dst=os.path.join(self.package_folder, "include"), src=self.source_folder)
        copy(self, pattern="include/spirv/unified1/*.h", dst=os.path.join(self.package_folder, "include"), keep_path=True, src=self.source_folder)
        copy(self, pattern="LICENSE*", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "spirv-reflect")
        self.cpp_info.set_property("cmake_target_name", "spirv-reflect::spirv-reflect")
        self.cpp_info.set_property("pkg_config_name", "spirv-reflect")
        self.cpp_info.libs = ["spirv-reflect-static"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include"]
