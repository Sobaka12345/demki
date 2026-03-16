macro (begin_project)

cmake_parse_arguments(__BEGIN_PROJECT
    _ VERSION EXTRA_SHADER_SOURCES ${ARGN}
)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

string(TOUPPER ${PROJECT_NAME} _CURRENT_PROJECT_NAME_TO_UPPER)

foreach(SHADER_PATH ${GENERATE_GLSLC_SCRIPT_SHADER_PATHS})
    LIST(APPEND SHADER_LIST -sf ${SHADER_PATH})
    LIST(JOIN SHADER_LIST " " SHADER_LIST)
endforeach()

cmake_language(EVAL CODE "
    set(${_CURRENT_PROJECT_NAME_TO_UPPER}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
")

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/models)
    cmake_language(EVAL CODE "
        set(${_CURRENT_PROJECT_NAME_TO_UPPER}_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
    ")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/textures)
    cmake_language(EVAL CODE "
        set(${_CURRENT_PROJECT_NAME_TO_UPPER}_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)
    ")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    cmake_language(EVAL CODE "
        set(${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    ")

    set(shader_header_template "${CMAKE_SOURCE_DIR}/templates/shared_types.hpp.in")
    set(shader_header_output   "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shared_types.hpp")
    if (EXISTS "${shader_header_template}")
        configure_file("${shader_header_template}" "${shader_header_output}" @ONLY)
    endif()

    file(GLOB_RECURSE "${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES" ABSOLUTE
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.vert"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.frag"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.comp"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.geom"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.tesc"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.tese"
        "${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADERS_DIR}/*.mesh"
    )
    set(${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES
        ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES} ${__BEGIN_PROJECT_EXTRA_SHADER_SOURCES})

    set(${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    set(${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_HPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders_hpp)
    file(MAKE_DIRECTORY ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_BIN_DIR})
    file(MAKE_DIRECTORY ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_HPP_DIR})

    foreach(shader_source ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES})
        get_filename_component(file_base_name ${shader_source} NAME)
        get_filename_component(shader_extension ${shader_source} EXT)
        string(TOLOWER "${shader_extension}" shader_stage)
        string(REGEX REPLACE "^\\." "" shader_stage "${shader_stage}")

        set(shader_bin ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_BIN_DIR}/${file_base_name}.spv)
        add_custom_command(OUTPUT ${shader_bin}
            COMMAND glslc
                ${shader_source}
                -fshader-stage=${shader_stage}
                -o ${shader_bin}
            DEPENDS ${shader_source}
            VERBATIM
        )

        set(generated_header ${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_HPP_DIR}/${file_base_name}.spv.hpp)
        add_custom_command(OUTPUT ${generated_header}
            COMMAND ${BIN2HEADER_EXECUTABLE}
                --output ${generated_header}
                ${shader_bin}
            DEPENDS ${shader_bin} ${shader_header_template} ${shader_header_output}
            VERBATIM
        )

        set(${_CURRENT_PROJECT_NAME_TO_UPPER}_GENERATED_SHADER_SOURCES
            ${generated_header} ${${_CURRENT_PROJECT_NAME_TO_UPPER}_GENERATED_SHADER_SOURCES})
    endforeach()

	set (${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES
		${${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES} ${${_CURRENT_PROJECT_NAME_TO_UPPER}_GENERATED_SHADER_SOURCES})
endif()

endmacro(begin_project)

macro (end_project)

if (EXISTS "${shader_header_output}")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLSL_HOST)
    target_precompile_headers(${PROJECT_NAME} PUBLIC ${shader_header_output})
endif()

if (DEFINED "${_CURRENT_PROJECT_NAME_TO_UPPER}_MODELS_DIR")
	message(${PROJECT_NAME} " models exist")
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E create_symlink "${${_CURRENT_PROJECT_NAME_TO_UPPER}_MODELS_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models")
endif()

if (DEFINED "${_CURRENT_PROJECT_NAME_TO_UPPER}_TEXTURES_DIR")
	message(${PROJECT_NAME} " textures exist")
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E create_symlink "${${_CURRENT_PROJECT_NAME_TO_UPPER}_TEXTURES_DIR}"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures")
endif()

if (DEFINED "${_CURRENT_PROJECT_NAME_TO_UPPER}_SHADER_SOURCES")
	message(${PROJECT_NAME} " shaders exist")
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CMAKE_CURRENT_BINARY_DIR}/shaders"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")

	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders DESTINATION ${PROJECT_NAME})
endif()


install (TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/")

endmacro (end_project)
