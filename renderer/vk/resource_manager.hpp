#pragma once

#include "buffer_shader_resource.hpp"

#include "handles/handle.hpp"
#include "handles/buffer.hpp"

#include "shader_interface_handle.hpp"

#include <iresource_manager.hpp>

#include <span>
#include <unordered_map>
#include <filesystem>

namespace vk {

class GraphicsContext;

struct Resource
{
	enum Storage : uint16_t
	{
		FILE = 0x0001,
		RAM = 0x0002,
		GPU = 0x0004
	};

	struct Descriptor
	{
		size_t id;
		Storage storage;
	};

	enum Type
	{
		Unknown = -1,
		Model = 0,
		Texture = 1
	} type;

	std::filesystem::path path;

	Resource() {}

    virtual ~Resource(){};
};

class ResourceManager : public IResourceManager
{
public:
    ResourceManager(const GraphicsContext& context);
    ~ResourceManager();

    virtual std::shared_ptr<IModel> createModel(std::span<const Vertex3DColoredTextured> vertices,
        std::span<const uint32_t> indices) override;

    virtual std::shared_ptr<IModel> createModel(std::filesystem::path path) override;
    virtual std::shared_ptr<ITexture> createTexture(ITexture::CreateInfo createInfo) override;

    std::shared_ptr<ShaderInterfaceHandle> fetchHandleSpecific(ShaderBlockType sbt,
        uint32_t layoutSize);
    virtual std::shared_ptr<IShaderInterfaceHandle> fetchHandle(ShaderBlockType sbt,
        uint32_t layoutSize) override;

private:
    uint32_t dynamicAlignment(uint32_t layoutSize) const;

private:
    const GraphicsContext& m_context;
	VkDeviceSize m_defaultBufferSize;

	std::vector<size_t> m_freeBufferSpace;
    handles::HandleVector<handles::Buffer> m_modelBuffers;
    handles::HandleVector<handles::Buffer> m_indBuffers;
    std::unordered_map<uint32_t, StaticUniformBufferShaderResource> m_staticUniformShaderResources;
    std::unordered_map<uint32_t, DynamicUniformBufferShaderResource>
        m_dynamicUniformShaderResources;
    std::unordered_map<uint32_t, StorageBufferShaderResource> m_storageShaderResources;
};

}    //  namespace vk
