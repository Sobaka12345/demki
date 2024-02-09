declare_project()

add_library(${PROJECT_NAME} STATIC
	abstract_application.hpp
	abstract_application.cpp
	graphical_application.hpp
	graphical_application.cpp
	qt_application.hpp
	qt_application.cpp
	update_timer.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)
