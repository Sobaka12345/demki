declare_project()

include(${CMAKE_SOURCE_DIR}/cmake_utils/project.cmake)
demki_init_target(
	TARGET          ${PROJECT_NAME}
    USE_QT			TRUE
	SHADER_SOURCES  ${SHARED_SHADERS_DIR}/shader.frag
                    ${SHARED_SHADERS_DIR}/shader.vert
)

demki_complete_target(
	TARGET      ${PROJECT_NAME}
    PUBLIC_LIBS engine
	SOURCES     main.cpp
                dummy.cpp
                dummy.hpp
)

demki_install_target(TARGET ${PROJECT_NAME})
