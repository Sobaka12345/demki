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

	qt/widget/widget.hpp
	qt/widget/widget.cpp
	qt/widget/vulkan_widget.hpp
	qt/widget/vulkan_widget.cpp
	qt/widget/opengl_widget.hpp
	qt/widget/opengl_widget.cpp

	qt/window/window.hpp
	qt/window/window.cpp
	qt/window/vulkan_window.hpp
	qt/window/vulkan_window.cpp
	qt/window/opengl_window.hpp
	qt/window/opengl_window.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
