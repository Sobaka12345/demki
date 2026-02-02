begin_project()


add_library(${PROJECT_NAME} STATIC 
    include/buffer.hpp
    include/gapi_context.hpp
    include/gapi.hpp
    include/surface.hpp
    include/swapchain.hpp

    ogl/gapi_context.hpp
    ogl/surface.cpp
    ogl/surface.hpp

    vk/buffer.cpp
    vk/buffer.hpp
    vk/gapi_context.cpp
    vk/gapi_context.hpp
    vk/swapchain.hpp
    vk/surface.hpp

    gapi_fwd.hpp
) 

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ./include/)
target_include_directories(${PROJECT_NAME} PRIVATE .)
