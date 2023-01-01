cmake_minimum_required(VERSION 3.15.4)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/install/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/install/conan_paths.cmake)

execute_process (
	COMMAND bash -c "bash ${CMAKE_SOURCE_DIR}/shaders/compile_shaders.sh ${CMAKE_SOURCE_DIR}/shaders  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
)

set(SHADER_SOURCES
	shaders/shader.frag
	shaders/shader.vert)

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
	renderer/buffer.hpp
	renderer/buffer.cpp
	renderer/uniform_buffer.hpp
	renderer/vertex.hpp
	renderer/utils.hpp
	renderer/creators.hpp
	renderer/model.cpp
	renderer/model.hpp
    renderer/graphical_application.hpp
    renderer/graphical_application.cpp
    tetris/tetris.hpp
    tetris/tetris.cpp
    tetris/figure.hpp
    tetris/figure.cpp
    ${SHADER_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan glm::glm)

execute_process (
	COMMAND bash -c "ln -s -f ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json"
)
