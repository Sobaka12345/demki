begin_project()

add_library(${PROJECT_NAME} STATIC
	update_timer.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)
