declare_project()

add_library(${PROJECT_NAME} INTERFACE
	assert.hpp
	hierarchy.hpp
	type_list.hpp
)

target_include_directories(${PROJECT_NAME} INTERFACE .)

