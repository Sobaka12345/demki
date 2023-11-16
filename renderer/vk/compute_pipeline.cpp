#include "compute_pipeline.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

namespace vk {

ComputePipelineCreateInfo ComputePipeline::defaultPipeline()
{
    return ComputePipelineCreateInfo().layout(VK_NULL_HANDLE).flags(0).pNext(nullptr);
}

ComputePipeline::ComputePipeline(const GraphicsContext& context, CreateInfo createInfo)
    : Pipeline(context, createInfo)
{}

ComputePipeline::~ComputePipeline() {}

const handles::Pipeline& ComputePipeline::pipeline(const OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    ASSERT(m_createInfo.shaders().size() == 1, "only compute shader is accepted");
    ASSERT(m_createInfo.shaders().back().type == ShaderType::COMPUTE,
        "only compute shader is accepted");

    handles::ShaderModule shader{ m_context.device(), m_createInfo.shaders().back().path };

    auto shaderStageCreateInfo =
        PipelineShaderStageCreateInfo{}
            .stage(toShaderStageFlags(ShaderType::COMPUTE))
            .module(shader)
            .pName("main");


    auto [newEl, _] = m_pipelines.emplace(context.renderPass,
        handles::ComputePipeline{ m_context.device(), VK_NULL_HANDLE,
            defaultPipeline().layout(*m_pipelineLayout).stage(shaderStageCreateInfo) });

    return newEl->second;
}

}    //  namespace vk
