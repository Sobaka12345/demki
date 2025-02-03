begin_project()

set(SANDBOX_TEXTURES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)
set(SANDBOX_TEXTURES_SRC ${SANDBOX_TEXTURES_DIR}/viking_room.jpg)

set(SANDBOX_MODELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/models)
set(SANDBOX_MODELS_SRC ${SANDBOX_TEXTURES_DIR}/viking_room.obj)

add_executable(${PROJECT_NAME}
    main.cpp
    types.h
    pch.hpp
    ${SANDBOX_SHADER_SOURCES}
    ${SANDBOX_TEXTURE_DIR})


target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp)

end_project()
