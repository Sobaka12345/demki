declare_project()

include(${CMAKE_SOURCE_DIR}/cmake_utils/project.cmake)
demki_init_target(
	TARGET          ${PROJECT_NAME}
	SHADER_SOURCES  ${SHARED_SHADERS_DIR}/shader.vert
                    ${SHARED_SHADERS_DIR}/shader.frag
)

demki_complete_target(
	TARGET      ${PROJECT_NAME}
    PUBLIC_LIBS engine
	SOURCES     main.cpp
                tetris.cpp
                tetris.hpp
                figure.hpp
                figure.cpp
                field.hpp
                field.cpp
                position.hpp
)

demki_install_target(TARGET ${PROJECT_NAME})