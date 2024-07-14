macro (declare_project)

cmake_minimum_required(VERSION 3.15.4)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

string(TOUPPER ${PROJECT_NAME} _CURRENT_PROJECT_NAME_TO_UPPER)
cmake_language(EVAL CODE "
    set(${_CURRENT_PROJECT_NAME_TO_UPPER}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
")

endmacro(declare_project)

declare_project()

include(cmake_utils/glslc.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL)
find_package(stb REQUIRED)
find_package(pfr REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(tclap REQUIRED)

set(SHARED_SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)

add_subdirectory(resources)
add_subdirectory(renderer)
add_subdirectory(shell)
add_subdirectory(engine)
add_subdirectory(utils)

#examples
add_subdirectory(examples/cubic)
add_subdirectory(examples/particles)
add_subdirectory(examples/tetris)
add_subdirectory(examples/dummy)

target_link_libraries(resources utils)
target_link_libraries(renderer PUBLIC resources vulkan-headers::vulkan-headers glad::glad glm::glm stb::stb tinyobjloader::tinyobjloader pfr::pfr tclap::tclap)
target_link_libraries(shell PUBLIC renderer glfw Qt6::Core Qt6::Widgets Qt6::Gui Qt6::OpenGL)
target_link_libraries(engine PUBLIC shell)
target_link_libraries(tetris PRIVATE engine)
target_link_libraries(cubic PRIVATE engine)
target_link_libraries(dummy PRIVATE engine)
target_link_libraries(particles PRIVATE engine)

add_custom_target(conan SOURCES conanfile.py configure_linux_default.sh configure_windows_default.bat )
source_group(config FILES conanfile.py configure_linux_default.sh configure_windows_default.bat)
