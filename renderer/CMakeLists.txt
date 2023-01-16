cmake_minimum_required(VERSION 3.15.4)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED
    ubo_value.hpp
    descriptor_set.hpp
    descriptor_pool.hpp
    descriptor_pool.cpp
    renderer.hpp
    renderer.cpp
    renderable.hpp
    renderable.cpp
    device.hpp
    device.cpp
    buffer.hpp
    buffer.cpp
    uniform_buffer.hpp
    vertex.hpp
    utils.hpp
    creators.hpp
    model.cpp
    model.hpp
    graphical_application.hpp
    graphical_application.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)
