#pragma once

#include "uniform_provider.hpp"

#include "handles/handle.hpp"
#include "handles/buffer.hpp"

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

    virtual std::shared_ptr<IUniformHandle> uniformHandle(uint32_t layoutSize) override;

private:
    uint32_t dynamicAlignment(uint32_t layoutSize) const;

private:
    const GraphicsContext& m_context;
	VkDeviceSize m_defaultBufferSize;

	std::vector<size_t> m_freeBufferSpace;
    handles::HandleVector<handles::Buffer> m_modelBuffers;
    handles::HandleVector<handles::Buffer> m_indBuffers;
    std::unordered_multimap<uint32_t, UniformProvider> m_uniformProviders;
};

}    //  namespace vk
