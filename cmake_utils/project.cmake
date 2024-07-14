macro (demki_init_target)
    set(options USE_QT)
    set(oneValueArgs TARGET)
    set(multiValueArgs SHADER_SOURCES)
    cmake_parse_arguments(_TARGET_VAR "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    set(CMAKE_INSTALL_BINDIR ${_TARGET_VAR_TARGET})

    set(${_TARGET_VAR_TARGET}_USE_QT ${_TARGET_VAR_USE_QT})
    set(${_TARGET_VAR_TARGET}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(${_TARGET_VAR_TARGET}_SHADER_SOURCES ${_TARGET_VAR_SHADER_SOURCES})
    set(${_TARGET_VAR_TARGET}_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
    set(${_TARGET_VAR_TARGET}_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)

    if (${_TARGET_VAR_TARGET}_USE_QT)
        qt_standard_project_setup()
    endif()
endmacro (demki_init_target)

macro (demki_complete_target)
    set(oneValueArgs TARGET)
    set(multiValueArgs SOURCES PUBLIC_LIBS PRIVATE_LIBS)
    cmake_parse_arguments(_TARGET_VAR "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})


    if (${_TARGET_VAR_TARGET}_USE_QT)
        qt_add_executable(${_TARGET_VAR_TARGET} ${_TARGET_VAR_SOURCES} ${${_TARGET_VAR_TARGET}_SHADER_SOURCES})
    else()
        add_executable(${_TARGET_VAR_TARGET} ${_TARGET_VAR_SOURCES} ${${_TARGET_VAR_TARGET}_SHADER_SOURCES})
    endif()

    target_link_libraries(${_TARGET_VAR_TARGET} PUBLIC ${_TARGET_VAR_PUBLIC_LIBS} PRIVATE ${_TARGET_VAR_PRIVATE_LIBS})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${${_TARGET_VAR_TARGET}_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
        LIBRARY_OUTPUT_DIRECTORY "${${_TARGET_VAR_TARGET}_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
        RUNTIME_OUTPUT_DIRECTORY "${${_TARGET_VAR_TARGET}_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
    )

    include (${CMAKE_SOURCE_DIR}/cmake_utils/glslc.cmake)
    message("\n-- Shader compilation for target " ${_TARGET_VAR_TARGET})
    generate_glslc_script(PROJECT_DIR ${${_TARGET_VAR_TARGET}_BINARY_DIR} SHADER_PATHS ${${_TARGET_VAR_TARGET}_SHADER_SOURCES})
    message("\n-- Shader compilation completed")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${${_TARGET_VAR_TARGET}_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${_TARGET_VAR_TARGET}>/shaders")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${${_TARGET_VAR_TARGET}_TEXTURES_DIR}" "$<TARGET_FILE_DIR:${_TARGET_VAR_TARGET}>/textures")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "${${_TARGET_VAR_TARGET}_MODELS_DIR}" "$<TARGET_FILE_DIR:${_TARGET_VAR_TARGET}>/models")
endmacro (demki_complete_target)


macro (demki_install_target)
    set(oneValueArgs TARGET)
    cmake_parse_arguments(_TARGET_VAR "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    install (DIRECTORY ${${_TARGET_VAR_TARGET}_TEXTURES_DIR} DESTINATION ${_TARGET_VAR_TARGET})
    install (DIRECTORY ${${_TARGET_VAR_TARGET}_MODELS_DIR} DESTINATION ${_TARGET_VAR_TARGET})
    install (DIRECTORY ${${_TARGET_VAR_TARGET}_BINARY_DIR}/shaders DESTINATION ${_TARGET_VAR_TARGET})
    install (TARGETS ${_TARGET_VAR_TARGET} DESTINATION ${_TARGET_VAR_TARGET})

    if (${_TARGET_VAR_TARGET}_USE_QT)
        set(DEPLOY_OPTIONS "")
        if (WIN32)
            set(DEPLOY_OPTIONS "--dir" ${_TARGET_VAR_TARGET} "--libdir" ${_TARGET_VAR_TARGET} "--plugindir" "${_TARGET_VAR_TARGET}/plugins")
        endif()

        qt_generate_deploy_script(
            TARGET ${_TARGET_VAR_TARGET}
            OUTPUT_SCRIPT deploy_script
            CONTENT "

            qt_deploy_runtime_dependencies(
                EXECUTABLE \"${_TARGET_VAR_TARGET}/$<TARGET_FILE_NAME:${_TARGET_VAR_TARGET}>\"
                BIN_DIR ${_TARGET_VAR_TARGET}
                LIB_DIR ${_TARGET_VAR_TARGET}
                PLUGINS_DIR ${_TARGET_VAR_TARGET}/plugins
                NO_TRANSLATIONS
                VERBOSE
                PRE_EXCLUDE_REGEXES
                    [=[api-ms-]=] # VC Redistibutable DLLs
                    [=[ext-ms-]=] # Windows extension DLLs
                    [=[[Qq]t[0-9]+[^\\/]*\.dll]=] # Qt Libs, don't block on Linux since users likely only have older Qt available
                POST_EXCLUDE_REGEXES
                    [=[.*system32\/.*\.dll]=] # Windows system DLLs
                    [=[^\/(lib|usr\/lib|usr\/local\/lib)]=] # Unix system libraries
            )"
        )

        install(SCRIPT ${deploy_script})
    endif()
endmacro (demki_install_target)