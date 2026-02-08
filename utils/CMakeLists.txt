begin_project()

add_library(${PROJECT_NAME} INTERFACE
	assert.hpp
	bytes.hpp
	constraints.hpp
	crtp.hpp
	glsl_defs.hpp
	glsl_defs.cpp
	hierarchy.hpp
	type_list.hpp
	pch.hpp
)

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} INTERFACE .)
