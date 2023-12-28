#include "model.hpp"

namespace ogl {

Model::Model(GraphicsContext& context, CreateInfo createInfo) noexcept
    : m_context(context)
{}

Model::~Model() {}

void Model::draw(OperationContext& context) {}

}
