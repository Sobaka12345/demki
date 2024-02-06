#pragma once

#include <igraphics_pipeline.hpp>
#include <utils.hpp>

#include <glad/glad.h>

namespace ogl {

class GraphicsContext;

class Pipeline 
    : virtual public IPipeline 
{
public:
    struct BindContext : public IPipelineBindContext
    {
        BindContext(std::span<const uint32_t> bindingIndices);
        virtual void bind(::OperationContext& context,
            const IShaderInterfaceContainer& container) override;

        std::span<const uint32_t> bindingIndices;
    };

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

    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

protected:
    const GraphicsContext& m_context;

    GLuint m_shaderProgram;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_bindingIndices;
    FragileSharedPtrMap<std::type_index, IPipelineBindContext> m_bindContexts;
};

}    //  namespace ogl
