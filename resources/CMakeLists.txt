declare_project()

add_library(${PROJECT_NAME} STATIC
	iresources.hpp
	iresource.hpp
	resources.hpp
	resources.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
