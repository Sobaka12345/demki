#pragma once

#include <igraphics_pipeline.hpp>

#include <glad/glad.h>

namespace ogl {

class GraphicsContext;

class GraphicsPipeline : public IGraphicsPipeline
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
    GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    int primitiveTopology() const;

    virtual void bind(::OperationContext& context) override;
    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

private:
    const GraphicsContext& m_context;
    bool m_isInDestruction;

    glm::float32_t m_sampleShading;
    int m_primitiveTopology;
    int m_cullMode;
    int m_frontFace;
    int m_polygonMode;

    GLuint m_shaderProgram;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_bindingIndices;
    std::unordered_map<std::type_index, FragileSharedPtr<BindContext>> m_bindContexts;
};

}    //  namespace ogl
