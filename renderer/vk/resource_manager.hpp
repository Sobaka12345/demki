#pragma once

#include "vertex.hpp"

#include "handles/handle.hpp"
#include "handles/buffer.hpp"

#include <span>
#include <filesystem>

class Model;

namespace vk {

namespace handles {
class Device;
}

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

class ResourceManager
{
public:
    ResourceManager(const handles::Device& device);

	std::shared_ptr<Model> loadModel(std::span<const Vertex3DColoredTextured> vertices,
		std::span<const uint32_t> indices);

    std::shared_ptr<Model> loadModel(std::filesystem::path path,
		Resource::Storage targetStorage = Resource::GPU);

private:
    const handles::Device& m_device;
	VkDeviceSize m_defaultBufferSize;

	std::vector<size_t> m_freeBufferSpace;
    handles::HandleVector<handles::Buffer> m_modelBuffers;
    handles::HandleVector<handles::Buffer> m_indBuffers;
};

}    //  namespace vk
