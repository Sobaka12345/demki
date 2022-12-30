#pragma once

#include "../renderer/graphical_application.hpp"
#include "../renderer/buffer.hpp"
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

    struct UniformBuffers
    {
        UniformBuffer m_viewProjectionBuffer;
        UniformBuffer m_modelBufffer;
    };
    VkDescriptorSet m_vkDescriptorSet;
    uint32_t m_dynamicAlignment;
    UniformBuffers m_uniformBuffers;
    glm::mat4x4* m_modelBuffers;
};
