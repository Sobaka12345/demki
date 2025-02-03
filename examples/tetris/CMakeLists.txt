begin_project()

set(TETRIS_SHADER_SOURCES
	${DEMKI_SHADERS_DIR}/shader.vert
	${DEMKI_SHADERS_DIR}/shader.frag)

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

end_project()
