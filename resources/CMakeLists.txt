begin_project()

add_library(${PROJECT_NAME} STATIC
	resources.hpp
	resources_vk.cpp
	resources.cpp
	plain_view.hpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
