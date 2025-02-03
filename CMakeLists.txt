cmake_minimum_required(VERSION 3.15.4)
set(QT_CREATOR_SKIP_CONAN_SETUP ON)
project(demki VERSION 0.1 LANGUAGES CXX)

include(cmake_utils/project.cmake)
include(cmake_utils/glslc.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED)
find_package(stb REQUIRED)
find_package(pfr REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(tclap REQUIRED)
#find_package(spirv-cross REQUIRED)
find_package(spirv-reflect REQUIRED)

find_package(bin2header REQUIRED)
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPERCASE)
set(BIN2HEADER_EXECUTABLE "bin2header_BIN_DIRS_${CMAKE_BUILD_TYPE_UPPERCASE}")
set(BIN2HEADER_EXECUTABLE "${${BIN2HEADER_EXECUTABLE}}/bin2header")

add_subdirectory(resources)
add_subdirectory(renderer)
add_subdirectory(shell)
add_subdirectory(engine)
add_subdirectory(utils)

#examples
# add_subdirectory(examples/cubic)
# add_subdirectory(examples/particles)
# add_subdirectory(examples/tetris)
add_subdirectory(examples/dummy)
# add_subdirectory(examples/arkanoid)
add_subdirectory(examples/sandbox)

target_link_libraries(utils INTERFACE glm::glm)
target_link_libraries(resources PUBLIC utils)
target_link_libraries(renderer PUBLIC bin2header::bin2header resources spirv-reflect::spirv-reflect glad::glad stb::stb tinyobjloader::tinyobjloader pfr::pfr tclap::tclap Vulkan::Vulkan)
target_link_libraries(shell PUBLIC renderer glfw qt::qt)
target_link_libraries(engine PUBLIC shell)
# target_link_libraries(tetris PRIVATE engine)
# target_link_libraries(cubic PRIVATE engine)
target_link_libraries(dummy PRIVATE engine)
# target_link_libraries(particles PRIVATE engine)
# target_link_libraries(arkanoid PRIVATE engine)
target_link_libraries(sandbox PRIVATE utils spirv-reflect::spirv-reflect glm::glm Vulkan::Vulkan)

add_custom_target(conan SOURCES
    conanfile.py
    configure_linux_default.sh
    configure_windows_default.bat
    conan/export.sh
    conan/export.bat
    conan/spirv_reflect/conanfile.py
    conan/bin2header/conanfile.py
)
