declare_project()

include(${CMAKE_SOURCE_DIR}/cmake_utils/project.cmake)
demki_init_target(
	TARGET 			${PROJECT_NAME}
	SHADER_SOURCES  ${SHARED_SHADERS_DIR}/shader.frag
					${SHARED_SHADERS_DIR}/shader.vert
)

demki_complete_target(
	TARGET 		${PROJECT_NAME}
	PUBLIC_LIBS engine
	SOURCES 	main.cpp
)

demki_install_target(TARGET ${PROJECT_NAME})
