begin_project()

add_library(${PROJECT_NAME} STATIC
	renderer.cpp

	include/default_renderer.hpp
	vk/default_renderer.cpp
	vk/default_renderer.hpp
	
	pch.hpp
	renderer_fwd.hpp
	${RENDERER_SHADER_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ./include/)
target_include_directories(${PROJECT_NAME} PRIVATE .)
