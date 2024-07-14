declare_project()

include(${CMAKE_SOURCE_DIR}/cmake_utils/project.cmake)
demki_init_target(
	TARGET 			${PROJECT_NAME}
	USE_QT			TRUE
	SHADER_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag
					${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert
					${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.comp
)

demki_complete_target(
	TARGET 		${PROJECT_NAME}
	PUBLIC_LIBS engine
	SOURCES		main.cpp
				particles_application.cpp
				particles_application.hpp
)

demki_install_target(TARGET ${PROJECT_NAME})