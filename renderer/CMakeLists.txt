begin_project()

add_library(${PROJECT_NAME} STATIC
	# ogl/computer.hpp
	# ogl/computer.cpp
	# ogl/compute_pipeline.hpp
	# ogl/compute_pipeline.cpp
	# ogl/graphics_pipeline.hpp
	# ogl/graphics_pipeline.cpp
	# ogl/graphics_context.hpp
	# ogl/graphics_context.cpp
	# ogl/ispecific_operation_target.hpp
	# ogl/mesh.hpp
	# ogl/mesh.cpp
	ogl/operation_context.hpp
	ogl/operation_context.cpp
	# ogl/pipeline.hpp
	# ogl/pipeline.cpp
	# ogl/renderer.hpp
	# ogl/renderer.cpp
	# ogl/storage_buffer.hpp
	# ogl/storage_buffer.cpp
	# ogl/swapchain.hpp
	# ogl/swapchain.cpp
	# ogl/texture.hpp
	# ogl/texture.cpp
	# ogl/uniform_buffer.hpp
	# ogl/uniform_buffer.cpp
	# ogl/utils.hpp
	vk/handles/allocator.hpp
	vk/handles/buffer.hpp
	vk/handles/command_buffer.hpp
	vk/handles/command_pool.hpp
	vk/handles/compute_pipeline.hpp
	vk/handles/debug_utils_messenger.hpp
	vk/handles/descriptor_set.hpp
	vk/handles/descriptor_set_layout.hpp
	vk/handles/descriptor_pool.hpp
	vk/handles/device.hpp
	vk/handles/fence.hpp
    vk/handles/framebuffer.hpp
    vk/handles/graphics_pipeline.hpp
    vk/handles/handle.hpp
    vk/handles/image.hpp
    vk/handles/image_view.hpp
    vk/handles/instance.hpp
    vk/handles/memory.hpp
    vk/handles/pipeline.hpp
    vk/handles/pipeline_layout.hpp
    vk/handles/physical_device.hpp
    vk/handles/queue.hpp
    vk/handles/render_pass.hpp
    vk/handles/sampler.hpp
    vk/handles/semaphore.hpp
    vk/handles/shader_module.hpp
    vk/handles/surface.hpp
    vk/handles/swapchain.hpp
    vk/uniform_set_pool.hpp
    vk/graphics_context.hpp
    vk/graphics_context.cpp
	vk/computer.hpp
	vk/computer.cpp
	vk/compute_pipeline.hpp
	vk/compute_pipeline.cpp
	vk/graphics_pipeline.hpp
	vk/graphics_pipeline.cpp
	vk/specific_pipeline.hpp
        vk/specific_pipeline.cpp
        vk/specific_buffer.hpp
        vk/specific_buffer.cpp
	vk/renderer.hpp
	vk/renderer.cpp
	vk/operation_context.hpp
	vk/operation_context.cpp
	vk/specific_context_object.hpp
	vk/specific_operation_target.hpp
	vk/storage_buffer.hpp
	vk/storage_buffer.cpp
    vk/swapchain.hpp
    vk/swapchain.cpp
    vk/types.hpp
    vk/uniform_buffer.hpp
    vk/uniform_buffer.cpp
    vk/utils.hpp
    include/ibuffer.hpp
	include/icomputer.hpp
	include/icompute_pipeline.hpp
	include/icompute_target.hpp
	include/icontext_object.hpp
	include/iuniform_set_pool.hpp
	include/igraphics_context.hpp
	include/igraphics_pipeline.hpp
	include/ipipeline.hpp
	include/ipool.hpp
	include/irenderer.hpp
	include/irender_target.hpp
	include/ioperation_target.hpp
	include/istorage_buffer.hpp
	include/iswapchain.hpp
	include/types.hpp
	include/ishader_interface.hpp
	include/ishader_interface_container.hpp
	include/ishader_interface_layout.hpp
	include/ishader_resource.hpp
	include/isurface.hpp
	include/ivulkan_surface.hpp
	include/iopengl_surface.hpp
	include/camera.hpp
	include/particles.hpp
	include/iuniform_buffer.hpp
	include/uniform_value.hpp
	camera.cpp
	create_info.cpp
	particles.cpp
	operation_context.hpp
	utils.hpp
	shader_interface.cpp
	pch.hpp
	${RENDERER_SHADER_SOURCES}
)

target_precompile_headers(${PROJECT_NAME} INTERFACE pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ./include/)
target_include_directories(${PROJECT_NAME} PRIVATE .)
