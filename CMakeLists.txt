cmake_minimum_required(VERSION 3.15.4)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/install/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/install/conan_paths.cmake)

execute_process (
	COMMAND bash -c "bash ${CMAKE_SOURCE_DIR}/shaders/compile_shaders.sh ${CMAKE_SOURCE_DIR}/shaders  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
    renderer/GraphicalApplication.hpp
    renderer/GraphicalApplication.cpp
    tetris/tetris.hpp
    tetris/tetris.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan glm::glm)

execute_process (
	COMMAND bash -c "ln -s -f ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json"
)
