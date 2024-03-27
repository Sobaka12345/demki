declare_project()

add_library(${PROJECT_NAME} STATIC
	iwindow.hpp
	iopengl_window.hpp
	ivulkan_window.hpp

	glfw/window.hpp
	glfw/window.cpp
	glfw/vulkan_window.hpp
	glfw/vulkan_window.cpp
	glfw/opengl_window.hpp
	glfw/opengl_window.cpp

	qt/window.hpp
	qt/window.cpp
	qt/vulkan_window.hpp
	qt/vulkan_window.cpp
	qt/opengl_window.hpp
	qt/opengl_window.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
