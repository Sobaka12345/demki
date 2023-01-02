#pragma once

#include "../renderer/graphical_application.hpp"
#include "../renderer/uniform_buffer.hpp"

#include <memory>

class Tetris: public GraphicalApplication
{
public:
    ~Tetris() override;

private:
    virtual void initApplication() override;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

    void createGraphicsPipeline();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();

    void updateUniformBuffer(uint32_t currentImage);
    void updateDynUniformBuffer(uint32_t currentImage);

private:
    VkPipeline m_vkPipeline;
    VkPipelineLayout m_vkPipelineLayout;

    VkDescriptorPool m_vkDescriptorPool;
    VkDescriptorSetLayout m_vkDescriptorSetLayout;

    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;
    std::unique_ptr<vk::UniformBuffer<vk::UBOModel>> m_modelBuffer;
    std::unique_ptr<vk::UniformBuffer<vk::UBOViewProjection>> m_viewProjectionBuffer;

    VkDescriptorSet m_vkDescriptorSet;
    vk::UBOModel* m_modelBuffers;
};
