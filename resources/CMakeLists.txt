begin_project()

add_library(${PROJECT_NAME} STATIC
	iresources.hpp
	iresource.hpp
	mesh.hpp
	mesh.cpp
	resources.hpp
	resources.cpp
	image.hpp
	image.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
