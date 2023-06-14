find_package(Python3 REQUIRED)

function(generate_glslc_script)
    cmake_parse_arguments(GENERATE_GLSLC_SCRIPT REQUIRED PROJECT_DIR
                          SHADER_PATHS ${ARGN})

    if(NOT GENERATE_GLSLC_SCRIPT_PROJECT_DIR)
        message(FATAL_ERROR "You must provide PROJECT_DIR")
    endif(NOT GENERATE_GLSLC_SCRIPT_PROJECT_DIR)

    if(NOT GENERATE_GLSLC_SCRIPT_SHADER_PATHS)
        message(FATAL_ERROR "You must provide SHADER_PATHS")
    endif(NOT GENERATE_GLSLC_SCRIPT_SHADER_PATHS)

    foreach(SHADER_PATH ${GENERATE_GLSLC_SCRIPT_SHADER_PATHS})
        LIST(APPEND SHADER_LIST -sf ${SHADER_PATH})
    endforeach()

    file(MAKE_DIRECTORY ${GENERATE_GLSLC_SCRIPT_PROJECT_DIR})

    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/generate_compile_shaders.py -df ${GENERATE_GLSLC_SCRIPT_PROJECT_DIR} ${SHADER_LIST}
    )
endfunction()