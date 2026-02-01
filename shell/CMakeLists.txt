begin_project()

add_library(${PROJECT_NAME} STATIC
	abstract/ogl/application.hpp
	abstract/ogl/application.cpp
	abstract/ogl/window.hpp
	abstract/vk/window.hpp
	abstract/vk/application.hpp
	abstract/vk/application.cpp
	abstract/application.hpp
	abstract/application.cpp
	abstract/window.hpp
	abstract/widget.hpp

	glfw/ogl/window.hpp
	#glfw/ogl/window.cpp
	glfw/vk/window.hpp
	#glfw/vk/window.cpp
	glfw/application.hpp
	glfw/application.cpp

	qt/ogl/widget.hpp
	qt/vk/widget.hpp
	qt/ogl/window.hpp
	qt/vk/window.hpp
	qt/window.hpp
	qt/window.cpp
	qt/widget.hpp
	qt/widget.cpp
	qt/application.hpp
	qt/application.cpp
 
	shell_fwd.hpp
	update_timer.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_include_directories(${PROJECT_NAME}
	PUBLIC .
)
