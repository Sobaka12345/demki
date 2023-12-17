#pragma once

#include "shader_interface.hpp"
#include "shader_interface_handle.hpp"

#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"

#include <ipipeline.hpp>

#include <vector>
#include <map>
#include <set>

namespace vk {

class Renderer;
class OperationContext;
class GraphicsContext;
class DescriptorSetProvider;

class ShaderInterfaceHandle;

namespace handles {
class Pipeline;
class PipelineLayout;
class RenderPass;
}

class Pipeline : virtual public IPipeline
{
    struct IdVectorHasher
    {
        size_t operator()(const std::vector<ShaderResource::Descriptor::Id>& e) const
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

protected:
    struct BindContext : public IPipelineBindContext
    {
        struct DescriptorSetInfo
        {
            uint32_t setId = 0;
            std::vector<uint32_t>& bindingIndices;
            DescriptorSetProvider& descriptorSetProvider;
            handles::DescriptorSetLayout& descriptorSetLayout;
        };

        BindContext(DescriptorSetInfo descriptorSetInfo);

        virtual void bind(::OperationContext& context,
            const IShaderInterfaceContainer& container) override;

    protected:
        DescriptorSetInfo descriptorSetInfo;
        std::weak_ptr<handles::DescriptorSet> currentSet;
        std::unordered_map<std::vector<ShaderResource::Descriptor::Id>,
            std::shared_ptr<handles::DescriptorSet>,
            ShaderResource::Descriptor::Id::ContainerHasher<std::vector>>
            sets;
    };

public:
    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

    const handles::PipelineLayout& layout() const { return *m_pipelineLayout; }

protected:
    Pipeline(const GraphicsContext& context, const auto& createInfo)
        : m_context(context)
        , m_isInDestruction(false)
    {
        init(createInfo.interfaceContainers());
    }

    ~Pipeline();

private:
    void init(const std::vector<InterfaceContainerInfo>& interfaceContainers);
    virtual BindContext* newBindContext(BindContext::DescriptorSetInfo descriptorSetInfo) const = 0;

protected:
    const GraphicsContext& m_context;

    bool m_isInDestruction;
    std::unordered_map<uint32_t, DescriptorSetProvider> m_descriptorSetProviders;
    std::list<FragileSharedPtr<BindContext>> m_bindContexts;
    std::unordered_map<uint32_t, std::pair<uint32_t, handles::DescriptorSetLayout>> m_setLayouts;

    std::unique_ptr<handles::PipelineLayout> m_pipelineLayout;

    std::unordered_map<uint32_t, std::vector<uint32_t>> m_bindingIndices;
    std::unordered_map<uint32_t, uint32_t> m_descriptorsCount;

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
