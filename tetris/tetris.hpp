#pragma once

#include <renderer.hpp>
#include <ubo_value.hpp>
#include <descriptor_pool.hpp>
#include <graphical_application.hpp>

#include <list>
#include <memory>
#include <functional>

class Model;
class Field;

class Tetris: public GraphicalApplication
{

public:
    Tetris();
    ~Tetris() override;

private:
    virtual void initApplication() override;
    virtual void update(int64_t dt) override;
    virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

    void createGraphicsPipeline();

    static void onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
    VkPipeline m_vkPipeline;
    VkPipelineLayout m_vkPipelineLayout;

    bool m_rotate;
    int32_t m_dx;
    int32_t m_flushedTotal;
    UpdateTimer m_gameTimer;
    UpdateTimer m_moveTimer;
    UpdateTimer m_rotationTimer;

    Renderer m_renderer;

    std::shared_ptr<vk::DescriptorSet> m_descriptorSet;
    std::unique_ptr<vk::DescriptorPool> m_descriptorPool;
    std::unique_ptr<vk::DescriptorSetLayout> m_descriptorSetLayout;

    std::shared_ptr<Model> m_cube;
    std::unique_ptr<vk::UBOValue<vk::UBOViewProjection>> m_camera;
    std::unique_ptr<vk::UniformBuffer<vk::UBOModel>> m_modelBuffer;
    std::unique_ptr<vk::UniformBuffer<vk::UBOViewProjection>> m_viewProjectionBuffer;

    std::shared_ptr<Field> m_field;
};
