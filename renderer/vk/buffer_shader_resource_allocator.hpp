#pragma once

#include "handles/buffer.hpp"

#include "shader_resource_allocator.hpp"

#include <iuniform_buffer.hpp>

#include <unordered_set>

namespace renderer::vk {

class GraphicsContext;

class BufferShaderResourceAllocator : public ShaderResourceAllocator
{
public:
    BufferShaderResourceAllocator(
        GraphicsContext& context, uint32_t alignment, uint32_t chunkObjectCount);
    virtual std::shared_ptr<ShaderResourceAllocator::Descriptor> fetchDescriptor() override;

    uint32_t alignment() const { return m_alignment; }

protected:
    virtual void populateDescriptor(ShaderResourceAllocator::Descriptor& descriptor);

private:
    size_t allocateBuffer();
    std::shared_ptr<ShaderResourceAllocator::Descriptor> tryFetchDescriptor(size_t bufferId);

    virtual void freeDescriptor(const ShaderResourceAllocator::Descriptor& descriptor) override;

    virtual handles::BufferCreateInfo bufferCreateInfo() const = 0;
    virtual VkMemoryPropertyFlags memoryProperties() const = 0;

protected:
    const uint32_t m_chunkObjectCount;
    const uint32_t m_alignment;

protected:
    handles::HandleVector<handles::Buffer> m_buffers;

private:
    GraphicsContext& m_context;

    std::vector<std::unordered_set<uint64_t>> m_freeDescriptors;
};

class UniformBufferShaderResourceAllocator : public BufferShaderResourceAllocator
{
public:
    using BufferShaderResourceAllocator::BufferShaderResourceAllocator;

protected:
    std::shared_ptr<ShaderInterfaceHandle> m_handle;

private:
    virtual handles::BufferCreateInfo bufferCreateInfo() const override;
    virtual VkMemoryPropertyFlags memoryProperties() const override;
};

class DynamicUniformBufferShaderResourceAllocator : public UniformBufferShaderResourceAllocator
{
public:
    using UniformBufferShaderResourceAllocator::UniformBufferShaderResourceAllocator;

private:
    virtual void populateDescriptor(ShaderResourceAllocator::Descriptor& descriptor) override;
};

class StaticUniformBufferShaderResourceAllocator : public UniformBufferShaderResourceAllocator
{
public:
    using UniformBufferShaderResourceAllocator::UniformBufferShaderResourceAllocator;

private:
    virtual void populateDescriptor(ShaderResourceAllocator::Descriptor& descriptor) override;
};

class StorageBufferShaderResourceAllocator : public BufferShaderResourceAllocator
{
public:
    using BufferShaderResourceAllocator::BufferShaderResourceAllocator;

private:
    virtual handles::BufferCreateInfo bufferCreateInfo() const override;
    virtual VkMemoryPropertyFlags memoryProperties() const override;
};

}    //  namespace renderer::vk
