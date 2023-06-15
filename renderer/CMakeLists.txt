declare_project()

add_library(${PROJECT_NAME} STATIC
    assert.hpp
    buffer.hpp
    buffer.cpp
    command_buffer.hpp
    command_buffer.cpp
    command_pool.hpp
    command_pool.cpp
    creators.hpp
    descriptor_set.hpp
    descriptor_pool.hpp
    descriptor_pool.cpp
    device.hpp
    device.cpp
    graphical_application.hpp
    graphical_application.cpp
    handle_base.hpp
    image.hpp
    image.cpp
    memory.hpp
    memory.cpp
    model.cpp
    model.hpp
    renderer.hpp
    renderer.cpp
    renderable.hpp
    renderable.cpp
    ubo_value.hpp
    uniform_buffer.hpp
    utils.hpp
    vertex.hpp
    texture.cpp
    texture.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)
