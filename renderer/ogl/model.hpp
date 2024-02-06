#pragma once

#include <imodel.hpp>

#include <glad/glad.h>

namespace ogl {

class GraphicsContext;

class Model : public IModel
{
public:
    explicit Model(GraphicsContext& context, IModel::CreateInfo createInfo) noexcept;
    virtual ~Model();

    virtual void bind(::OperationContext& context) override;
    virtual void draw(::OperationContext& context) override;

private:
    GraphicsContext& m_context;

    GLuint m_indexCount;

    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_vao;
};

}    //  namespace ogl
