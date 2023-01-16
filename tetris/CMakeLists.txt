cmake_minimum_required(VERSION 3.15.4)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp
    tetris.hpp
    tetris.cpp
    figure.hpp
    figure.cpp
    field.hpp
    field.cpp
    position.hpp
    ${SHADER_SOURCES})
