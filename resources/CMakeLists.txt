begin_project()

add_library(${PROJECT_NAME} STATIC
	iresources.hpp
	iresource.hpp
	resources.hpp
	resources.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
