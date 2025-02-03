begin_project()

add_executable(${PROJECT_NAME}
    main.cpp
	particles_application.cpp
	particles_application.hpp
	${PARTICLES_SHADER_SOURCES})

end_project()
