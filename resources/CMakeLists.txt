begin_project()

add_library(${PROJECT_NAME} STATIC
	resource.hpp
	resource.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
