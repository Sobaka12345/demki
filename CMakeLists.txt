cmake_minimum_required(VERSION 3.15.4)
set(QT_CREATOR_SKIP_CONAN_SETUP ON)
project(demki VERSION 0.1 LANGUAGES CXX)

include(cmake_utils/project.cmake)
include(cmake_utils/glslc.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # additional warnings
    add_compile_options(-fdiagnostics-color=never -Wno-missing-field-initializers)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fno-color-diagnostics)
endif()

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

add_subdirectory(gapi)
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
# add_subdirectory(examples/sandbox)

target_link_libraries(utils INTERFACE glm::glm)
target_link_libraries(gapi PUBLIC utils Vulkan::Vulkan glad::glad spirv-reflect::spirv-reflect )
target_link_libraries(renderer PUBLIC gapi bin2header::bin2header stb::stb tinyobjloader::tinyobjloader pfr::pfr tclap::tclap)
# target_link_libraries(resources PUBLIC renderer)
target_link_libraries(shell PUBLIC renderer glfw qt::qt)
target_link_libraries(engine PUBLIC shell)
# target_link_libraries(tetris PRIVATE engine)
# target_link_libraries(cubic PRIVATE engine)
target_link_libraries(dummy PRIVATE engine)
# target_link_libraries(particles PRIVATE engine)
# target_link_libraries(arkanoid PRIVATE engine)
# target_link_libraries(sandbox PRIVATE utils spirv-reflect::spirv-reflect glm::glm glfw Vulkan::Vulkan)

add_custom_target(conan SOURCES
    conanfile.py
    configure_linux_default.sh
    configure_windows_default.bat
    conan/export.sh
    conan/export.bat
    conan/spirv_reflect/conanfile.py
    conan/bin2header/conanfile.py
    conan/bin2header/conandata.yml
)
