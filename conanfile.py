from conans import ConanFile, CMake

class DemkiConan(ConanFile):
   settings = "os", "compiler", "build_type", "arch"
   requires = ["glfw/3.3.8", "vulkan-loader/1.3.231.1", "glm/0.9.9.8"]
   generators = ["cmake", "cmake_find_package", "cmake_paths"]

   def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()
