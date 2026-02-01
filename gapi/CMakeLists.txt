begin_project()


add_library(${PROJECT_NAME} STATIC 
    include/buffer.hpp
    include/device.hpp
    include/fence.hpp
    include/gapi_context.hpp
    include/gapi.hpp
    include/semaphore.hpp
    include/surface.hpp
    include/swapchain.hpp

    ogl/gapi_context.hpp
    ogl/surface.cpp
    ogl/surface.hpp

    vk/buffer.cpp
    vk/buffer.hpp
    vk/device.cpp
    vk/device.hpp
    vk/fence.cpp
    vk/fence.hpp
    vk/gapi_context.cpp
    vk/gapi_context.hpp
    vk/swapchain.hpp
    vk/semaphore.cpp
    vk/semaphore.hpp
    vk/surface.hpp

    gapi_fwd.hpp
) 

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ./include/)
target_include_directories(${PROJECT_NAME} PRIVATE .)
