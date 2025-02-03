begin_project()

set(ARKANOID_SHADER_SOURCES
	${DEMKI_SHADERS_DIR}/shader.frag
	${DEMKI_SHADERS_DIR}/shader.vert)

add_executable(${PROJECT_NAME}
	main.cpp
	arkanoid.cpp
	arkanoid.hpp
	${ARKANOID_SHADER_SOURCES}
	${ARKANOID_TEXTURE_DIR})

end_project()
