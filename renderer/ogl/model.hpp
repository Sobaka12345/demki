#pragma once

#include <imodel.hpp>

namespace ogl {

class GraphicsContext;

class Model : public IModel
{
public:
    explicit Model(GraphicsContext& context, IModel::CreateInfo createInfo) noexcept;
    virtual ~Model();

    virtual void bind(OperationContext& context) override;
    virtual void draw(OperationContext& context) override;

private:
    GraphicsContext& m_context;
};

}    //  namespace ogl
