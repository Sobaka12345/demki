#pragma once

#include <imesh.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

class GraphicsContext;

class Mesh : public IMesh
{
public:
    explicit Mesh(GraphicsContext& context, IMesh::CreateInfo createInfo) noexcept;
    virtual ~Mesh();

    virtual void bind(renderer::OperationContext& context) override;
    virtual void draw(renderer::OperationContext& context) override;

private:
    GraphicsContext& m_context;

    GLuint m_indexCount;

    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_vao;
};

}    //  namespace renderer::ogl
