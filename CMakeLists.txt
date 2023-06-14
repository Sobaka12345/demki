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

find_package(stb REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

set(SHARED_SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)

add_subdirectory(renderer)
add_subdirectory(tetris)
add_subdirectory(dummy)

target_link_libraries(renderer PUBLIC glfw Vulkan::Vulkan glm::glm stb::stb)
target_link_libraries(tetris PRIVATE renderer)
target_link_libraries(dummy PRIVATE renderer)

execute_process (
    COMMAND bash -c "ln -s ${CMAKE_SOURCE_DIR}/textures ${tetris_BINARY_DIR}/textures"
)

execute_process (
    COMMAND bash -c "ln -s -f ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json"
)
