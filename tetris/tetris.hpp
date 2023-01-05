#pragma once

#include "../renderer/renderer.hpp"
#include "../renderer/ubo_value.hpp"
#include "../renderer/descriptor_pool.hpp"
#include "../renderer/graphical_application.hpp"

#include <memory>
#include <functional>

class Model;
class Field;
class Figure;

class Tetris: public GraphicalApplication
{
public:
    Tetris();
    ~Tetris() override;

private:
    virtual void initApplication() override;
    virtual void update(int64_t dt) override;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

    static std::shared_ptr<Figure> figure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);

    void createGraphicsPipeline();

private:
    VkPipeline m_vkPipeline;
    VkPipelineLayout m_vkPipelineLayout;

    uint32_t m_timer;
    Renderer m_renderer;

    std::shared_ptr<vk::DescriptorSet> m_descriptorSet;
    std::unique_ptr<vk::DescriptorPool> m_descriptorPool;
    std::unique_ptr<vk::DescriptorSetLayout> m_descriptorSetLayout;

    std::shared_ptr<Model> m_cube;
    std::unique_ptr<vk::UBOValue<vk::UBOViewProjection>> m_camera;
    std::unique_ptr<vk::UniformBuffer<vk::UBOModel>> m_modelBuffer;
    std::unique_ptr<vk::UniformBuffer<vk::UBOViewProjection>> m_viewProjectionBuffer;

    std::shared_ptr<Field> m_field;
    std::list<std::shared_ptr<Figure>> m_blocks;
};
