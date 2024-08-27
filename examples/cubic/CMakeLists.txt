begin_project()

set(CUBIC_SHADER_SOURCES
	${DEMKI_SHADERS_DIR}/shader.frag
	${DEMKI_SHADERS_DIR}/shader.vert)

add_executable(${PROJECT_NAME}
    main.cpp
	cubic.cpp
	cubic.hpp
	${CUBIC_SHADER_SOURCES}
	${CUBIC_TEXTURE_DIR})


end_project()
