declare_project()

set(PARTICLES_SHADER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.comp)

set(PARTICLES_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)

set(PARTICLES_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)

add_executable(${PROJECT_NAME}
    main.cpp
	particles_application.cpp
	particles_application.hpp
	${PARTICLES_SHADER_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${PARTICLES_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	LIBRARY_OUTPUT_DIRECTORY "${PARTICLES_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	RUNTIME_OUTPUT_DIRECTORY "${PARTICLES_BINARY_DIR}/${CMAKE_BUILD_TYPE}/")

generate_glslc_script(PROJECT_DIR ${PARTICLES_BINARY_DIR} SHADER_PATHS ${PARTICLES_SHADER_SOURCES})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PARTICLES_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PARTICLES_TEXTURES_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures")

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E create_symlink "${PARTICLES_MODELS_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models")

install (DIRECTORY ${PARTICLES_BINARY_DIR}/shaders DESTINATION ${PROJECT_NAME})
install (TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
