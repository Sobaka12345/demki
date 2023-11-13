#pragma once

#include "shader_interface.hpp"
#include "shader_interface_handle.hpp"

#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"

#include <ipipeline.hpp>

#include <map>
#include <set>

namespace vk {

class Renderer;
class RenderContext;
class GraphicsContext;

class ShaderInterfaceHandle;

namespace handles {
class DescriptorPool;
class Pipeline;
class PipelineLayout;
class RenderPass;
}

class Pipeline : public IPipeline
{
    struct SetHasher
    {
        size_t operator()(const std::set<ShaderResource::Descriptor::Id>& e) const
        {
            size_t result = 0;
            uint64_t pow = 1;
            for (ShaderResource::Descriptor::Id el : e)
            {
                result += el.resourceId * pow + el.descriptorId * pow * 2 + el.bufferId * pow * 3;
                pow *= 10;
            }

            return result;
        }
    };

    struct BindContext : public IPipeline::IBindContext
    {
        virtual void bind(::RenderContext& context,
            const IShaderInterfaceContainer& container) override;
        std::shared_ptr<handles::DescriptorSet> set;
        uint32_t setId;
    };

public:
    Pipeline(const GraphicsContext& context, IPipeline::CreateInfo createInfo);
    ~Pipeline();

    virtual void bind(::RenderContext& context) override;
    virtual std::weak_ptr<IBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

    const handles::PipelineLayout& layout() const { return *m_pipelineLayout; }

private:
    virtual const handles::Pipeline& pipeline(const vk::RenderContext& context) = 0;

protected:
    const GraphicsContext& m_context;

    std::unique_ptr<handles::DescriptorPool> m_pool;
    std::unordered_map<std::set<ShaderResource::Descriptor::Id>,
        std::shared_ptr<BindContext>,
        SetHasher>
        m_bindContexts;
    std::unordered_map<uint32_t, std::pair<uint32_t, handles::DescriptorSetLayout>> m_setLayouts;

    std::unique_ptr<handles::PipelineLayout> m_pipelineLayout;

    std::unordered_map<InterfaceBlockID, uint32_t> m_descriptorsCount;

    IPipeline::CreateInfo m_createInfo;

    std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
};

inline VkShaderStageFlagBits toShaderStageFlags(IPipeline::ShaderType type)
{
    switch (type)
    {
        case ShaderStage::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        default: ASSERT(false, "unknown shader type");
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

inline VkDescriptorType toDescriptorType(ShaderBlockType type)
{
    switch (type)
    {
        case ShaderBlockType::UNIFORM_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case ShaderBlockType::UNIFORM_STATIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case ShaderBlockType::SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case ShaderBlockType::STORAGE: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default: ASSERT(false, "unknown descriptor type");
    };

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

}    //  namespace vk
