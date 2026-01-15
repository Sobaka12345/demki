from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, apply_conandata_patches, export_conandata_patches
from conan.tools.scm import Git
import os

required_conan_version = ">=2.0"


class Bin2HeaderConan(ConanFile):
    name = "bin2header"
    description = "Binary file converter to C/C++ source header."
    homepage = "https://github.com/AntumDeluge/bin2header"
    topics = ("binary-tool", "build-tool", "code-generator")
    license = "The MIT License"
    author = "Copyright © 2017-2022 Jordan Irwin (AntumDeluge)"
    package_type = "application"
    settings = "os", "arch", "compiler", "build_type"

    options = {
    }

    default_options = {
    }

    short_paths = True

    def layout(self):
        layout_folder = f"bin2header-{self.version}"
        cmake_layout(self, src_folder=layout_folder, build_folder=layout_folder)

    def export_sources(self):
        export_conandata_patches(self)

    def source(self):
        git = Git(self)
        git.clone("https://github.com/Sobaka12345/bin2header.git", target=".")
        #git.folder = "bin2header"
        #get(self, f'https://github.com/AntumDeluge/bin2header/archive/refs/tags/v{self.version}.tar.gz', strip_root=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["NATIVE"] = True
        tc.cache_variables["STATIC"] = True
        tc.cache_variables["EMBED_ICON"] = True
        tc.cache_variables["CMAKE_POLICY_VERSION_MINIMUM"] = "3.5"
        tc.generate()

    def build(self):
        apply_conandata_patches(self)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="LICENSE*", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.bindirs = ["bin"]
