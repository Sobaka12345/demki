#include "storage_buffer.hpp"

namespace ogl {

StorageBuffer::StorageBuffer(GraphicsContext& context, CreateInfo createInfo)
    : m_context(context)
{}

StorageBuffer::~StorageBuffer() {}

void StorageBuffer::accept(ComputerInfoVisitor& visitor) const {}

bool StorageBuffer::prepare(OperationContext& context) {}

void StorageBuffer::present(OperationContext& context) {}

std::weak_ptr<IShaderInterfaceHandle> StorageBuffer::handle() const {}

void StorageBuffer::bind(OperationContext& context) const {}

void StorageBuffer::draw(OperationContext& context) const {}

}    //  namespace ogl
