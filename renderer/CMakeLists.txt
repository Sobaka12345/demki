begin_project()

add_library(${PROJECT_NAME} STATIC
	vk/default_renderer.cpp
	vk/renderer.hpp
	
	pch.hpp
	renderer_fwd.hpp
	${RENDERER_SHADER_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ./include/)
target_include_directories(${PROJECT_NAME} PRIVATE .)
