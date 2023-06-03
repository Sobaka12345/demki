cmake_minimum_required(VERSION 3.15.4)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(stb REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

set(SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/shaders/shader.frag
    ${CMAKE_SOURCE_DIR}/shaders/shader.vert)

add_subdirectory(renderer)
add_subdirectory(tetris)

target_link_libraries(renderer PUBLIC glfw Vulkan::Vulkan glm::glm stb::stb)
target_link_libraries(tetris PRIVATE renderer)

execute_process (
    COMMAND bash -c "bash ${CMAKE_SOURCE_DIR}/shaders/compile_shaders.sh ${CMAKE_SOURCE_DIR}/shaders  ${tetris_BINARY_DIR}/shaders"
)

execute_process (
    COMMAND bash -c "ln -s ${CMAKE_SOURCE_DIR}/textures ${tetris_BINARY_DIR}/textures"
)

execute_process (
    COMMAND bash -c "ln -s -f ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json"
)
