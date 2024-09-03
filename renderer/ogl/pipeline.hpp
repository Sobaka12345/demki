#pragma once

#include <igraphics_pipeline.hpp>
#include <utils.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

class GraphicsContext;

struct PipelineBindContext : public IPipelineBindContext
{
    PipelineBindContext(IShaderInterfaceContainer& container,
        std::span<const uint32_t> bindingIndices);
    virtual void bind(renderer::OperationContext& context) override;

    std::span<const uint32_t> bindingIndices;
    IShaderInterfaceContainer& container;
};

class Pipeline : virtual public IPipeline
{
public:
    template <typename CreateInfoT>
    Pipeline(const GraphicsContext& context, CreateInfoT createInfo)
        : m_context(context)
    {
        init(createInfo.interfaceContainers(), createInfo.shaders());
    }

    virtual ~Pipeline();

    void init(const std::vector<InterfaceContainerInfo>& interfaceContainers,
        const std::vector<ShaderInfo>& shaders);

    virtual std::shared_ptr<IPipelineBindContext> bindContext(
        IShaderInterfaceContainer& container) override;

protected:
    const GraphicsContext& m_context;

    GLuint m_shaderProgram;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_bindingIndices;
    FragileSharedPtrMap<size_t, IPipelineBindContext> m_bindContexts;
};

}    //  namespace renderer::ogl
