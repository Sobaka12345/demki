declare_project()

set(TETRIS_SHADER_SOURCES
    ${SHARED_SHADERS_DIR}/shader.vert
    ${SHARED_SHADERS_DIR}/shader.frag)

set(TETRIS_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)
set(TETRIS_TEXTURES_SRC ${TETRIS_TEXTURES_DIR}/roshi.jpg)

add_executable(${PROJECT_NAME} main.cpp
    tetris.hpp
    tetris.cpp
    figure.hpp
    figure.cpp
    field.hpp
    field.cpp
    position.hpp
    ${TETRIS_SHADER_SOURCES}
    ${TETRIS_TEXTURES_SRC}
)


set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${TETRIS_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
    LIBRARY_OUTPUT_DIRECTORY "${TETRIS_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
    RUNTIME_OUTPUT_DIRECTORY "${TETRIS_BINARY_DIR}/${CMAKE_BUILD_TYPE}/")

generate_glslc_script(PROJECT_DIR ${TETRIS_BINARY_DIR} SHADER_PATHS ${TETRIS_SHADER_SOURCES})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E create_symlink "${TETRIS_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E create_symlink "${TETRIS_TEXTURES_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures")

install (DIRECTORY ${TETRIS_BINARY_DIR}/shaders DESTINATION ${PROJECT_NAME})
install (TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})