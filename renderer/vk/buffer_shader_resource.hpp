#pragma once

#include "handles/buffer.hpp"

#include "shader_interface_handle.hpp"
#include "shader_resource.hpp"

#include <map>
#include <unordered_set>

namespace vk {

class BufferShaderResource : public ShaderResource
{
public:
    BufferShaderResource(
        const handles::Device& device, uint32_t alignment, uint32_t chunkObjectCount);
    virtual std::shared_ptr<ShaderResource::Descriptor> fetchDescriptor() override;

    uint32_t alignment() const { return m_alignment; }

protected:
    virtual void populateDescriptor(ShaderResource::Descriptor& descriptor);

private:
    size_t allocateBuffer();
    std::shared_ptr<ShaderResource::Descriptor> tryFetchDescriptor(size_t bufferId);

    virtual void freeDescriptor(const ShaderResource::Descriptor& descriptor) override;

    virtual handles::BufferCreateInfo bufferCreateInfo() const = 0;
    virtual VkMemoryPropertyFlags memoryProperties() const = 0;

protected:
    const uint32_t m_chunkObjectCount;
    const uint32_t m_alignment;

protected:
    handles::HandleVector<handles::Buffer> m_buffers;

private:
    const handles::Device& m_device;

    std::vector<std::unordered_set<uint64_t>> m_freeDescriptors;
};

class UniformBufferShaderResource : public BufferShaderResource
{
public:
    using BufferShaderResource::BufferShaderResource;

private:
    virtual handles::BufferCreateInfo bufferCreateInfo() const override;
    virtual VkMemoryPropertyFlags memoryProperties() const override;
};

class DynamicUniformBufferShaderResource : public UniformBufferShaderResource
{
public:
    using UniformBufferShaderResource::UniformBufferShaderResource;

private:
    virtual void populateDescriptor(ShaderResource::Descriptor& descriptor) override;
};

class StaticUniformBufferShaderResource : public UniformBufferShaderResource
{
public:
    using UniformBufferShaderResource::UniformBufferShaderResource;

private:
    virtual void populateDescriptor(ShaderResource::Descriptor& descriptor) override;
};

class StorageBufferShaderResource : public BufferShaderResource
{
public:
    using BufferShaderResource::BufferShaderResource;

private:
    virtual handles::BufferCreateInfo bufferCreateInfo() const override;
    virtual VkMemoryPropertyFlags memoryProperties() const override;
};

}    //  namespace vk
