begin_project()

set(DUMMY_SHADER_SOURCES
	${DEMKI_SHADERS_DIR}/shader.frag
	${DEMKI_SHADERS_DIR}/shader.vert)

set(DUMMY_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)
set(DUMMY_TEXTURES_SRC ${DUMMY_TEXTURES_DIR}/viking_room.jpg)

set(DUMMY_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
set(DUMMY_MODELS_SRC ${DUMMY_TEXTURES_DIR}/viking_room.obj)

add_executable(${PROJECT_NAME}
    main.cpp
    dummy.cpp
    dummy.hpp
    ${DUMMY_SHADER_SOURCES}
    ${DUMMY_TEXTURE_DIR})

end_project()
