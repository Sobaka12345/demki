#include "tetris.hpp"

#include "field.hpp"

#include "../renderer/model.hpp"
#include "../renderer/vertex.hpp"
#include "../renderer/creators.hpp"

#include <iostream>

using namespace vk;

class TetrisDSL : public DescriptorSetLayout
{
public:
    constexpr static VkDescriptorSetLayoutBinding s_modelBinding =
        create::setLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 0);
    constexpr static VkDescriptorSetLayoutBinding s_cameraBinding =
        create::setLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

    constexpr static std::array s_layoutBindings = { s_modelBinding, s_cameraBinding };

public:
    TetrisDSL(const Device& device)
        : DescriptorSetLayout(device, create::descriptorSetLayoutCreateInfo(s_layoutBindings))
    {}
};

class TetrisDP : public DescriptorPool
{
public:
    TetrisDP(const Device& device)
        : DescriptorPool(device, 2,
            std::array<VkDescriptorPoolSize, 2> {
                create::descriptorPoolSize(TetrisDSL::s_modelBinding.descriptorType, 1),
                create::descriptorPoolSize(TetrisDSL::s_cameraBinding.descriptorType, 1),
            }
        )
    {}
};

class TetrisCamera : public UBOValue<UBOViewProjection>
{
public:
    TetrisCamera(const DescriptorSet* descriptorSet, std::shared_ptr<UBOHandler> handler)
        : UBOValue<UBOViewProjection>(descriptorSet, handler)
    {
    }
};

static constexpr std::array<Vertex3DColored, 8> s_cubeVertices = {
    Vertex3DColored{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}, //0
    Vertex3DColored{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, //1
    Vertex3DColored{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}, //2
    Vertex3DColored{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}, //3
    Vertex3DColored{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //4
    Vertex3DColored{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //5
    Vertex3DColored{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, //6
    Vertex3DColored{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}  //7
};

static constexpr std::array<uint16_t, 36> s_cubeIndices = {
        // Top
        7, 6, 2,
        2, 3, 7,
        // Bottom
        0, 4, 5,
        5, 1, 0,
        // Left
        0, 2, 6,
        6, 4, 0,
        // Right
        7, 3, 1,
        1, 5, 7,
        // Front
        3, 2, 0,
        0, 1, 3,
        // Back
        4, 6, 7,
        7, 5, 4
};

Tetris::Tetris()
    : m_dx(0)
    , m_rotate(false)
    , m_flushedTotal(0)
{
    m_gameTimer.setIntervalMS(500);
    m_moveTimer.setIntervalMS(150);
    m_rotationTimer.setIntervalMS(200);
}

Tetris::~Tetris()
{
    vkDestroyPipeline(*m_device, m_vkPipeline, nullptr);
    vkDestroyPipelineLayout(*m_device, m_vkPipelineLayout, nullptr);
}

void Tetris::initApplication()
{
    glfwSetKeyCallback(m_window, onKeyPressed);

    m_descriptorSetLayout = std::make_unique<TetrisDSL>(*m_device);

    // Render Pipeline
    createGraphicsPipeline();

    // Resources
    m_cube = Model::create(
        createAndWriteGPUBuffer<VertexBufferT, Vertex3DColored>(s_cubeVertices),
        createAndWriteGPUBuffer<IndexBufferT, uint16_t>(s_cubeIndices)
    );

    m_modelBuffer = std::make_unique<UniformBuffer<UBOModel>>(*m_device, Field::s_objectsCount);
    m_modelBuffer
        ->allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        ->map();

    m_viewProjectionBuffer = std::make_unique<UniformBuffer<UBOViewProjection>>(*m_device, 1);
    m_viewProjectionBuffer
        ->allocateAndBindMemory(
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        ->map();

    m_descriptorPool = std::make_unique<TetrisDP>(*m_device);
    m_descriptorSet = m_descriptorPool->allocateSet(*m_descriptorSetLayout, m_vkPipelineLayout);
    m_descriptorSet->write(std::array{
        DescriptorSet::Write{m_modelBuffer->descriptorBufferInfo(), TetrisDSL::s_modelBinding},
        DescriptorSet::Write{m_viewProjectionBuffer->descriptorBufferInfo(), TetrisDSL::s_cameraBinding},
    });

    // TO DO: MOVE TO CAMERA CLASS
    {
        m_camera = std::make_unique<TetrisCamera>(
            m_descriptorSet.get(),
            m_viewProjectionBuffer->tryGetUBOHandler());

        const float fov = 45.0f;
        const float distance = Field::s_height * 90.0f / fov;
        UBOViewProjection viewProjection;
        viewProjection.view = glm::lookAt(
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, distance),
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        viewProjection.projection = glm::perspective(
            glm::radians(fov),
            m_vkSwapChainExtent.width / static_cast<float>(m_vkSwapChainExtent.height),
            0.1f,
            distance + 1.0f);
        m_camera->set(viewProjection);
    }

    m_field = std::make_shared<Field>(m_descriptorSet.get(), m_modelBuffer.get());
    m_field->setModel(m_cube);
    m_renderer.pushObject(m_field);

    m_field->flushRowsAndSpawnFigure();
}

void Tetris::onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Tetris* thisPtr = reinterpret_cast<Tetris*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        thisPtr->m_rotate = true;
        thisPtr->m_field->tryRotateFigure();
        thisPtr->m_rotationTimer.reset(thisPtr->m_rotationTimer.interval());
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        thisPtr->m_rotate = false;
    }
    else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
    {
        thisPtr->m_dx += (key == GLFW_KEY_RIGHT) - (key == GLFW_KEY_LEFT);
        thisPtr->m_field->tryMoveFigure(thisPtr->m_dx, 0);
        thisPtr->m_moveTimer.reset();
    }
    else if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) && action == GLFW_RELEASE)
    {
        thisPtr->m_dx += -(key == GLFW_KEY_RIGHT) + (key == GLFW_KEY_LEFT);
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        thisPtr->m_gameTimer.setSpeedUp(2.0f);
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    {
        thisPtr->m_gameTimer.setNormalSpeed();
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        while (thisPtr->m_field->tryMoveFigure(0, 1));
    }
}

void Tetris::update(int64_t dt)
{
    if (m_rotate && m_rotationTimer.timePassed(dt))
    {
        m_field->tryRotateFigure();
    }
    if (m_dx != 0 && m_moveTimer.timePassed(dt))
    {
        m_field->tryMoveFigure(m_dx, 0);
    }

    if (m_gameTimer.timePassed(dt))
    {
        if (!m_field->tryMoveFigure(0, 1))
        {
            if (m_field->isBlocksOverflow())
            {
                std::cout << "YOU ARE A LOSER! TOTAL FLUSHED: " << m_flushedTotal << std::endl;
                glfwSetWindowShouldClose(m_window, GL_TRUE);
            }
            const int32_t flushed = m_field->flushRowsAndSpawnFigure();
            m_flushedTotal += flushed;
            std::cout << "FLUSHED ROWS COUNT: " << flushed << " TOTAL: " << m_flushedTotal << std::endl;

            if (flushed && m_flushedTotal % 8 == 0)
            {
                m_gameTimer.setInterval(m_gameTimer.interval() * 0.8f);
            }
        }
    }
}

// TO DO: POSSIBLY MOVE COMMAND BUFFER TO Renderer CLASS?
void Tetris::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_vkRenderPass;
    renderPassInfo.framebuffer = m_vkSwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vkSwapChainExtent.width);
    viewport.height = static_cast<float>(m_vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_vkSwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    m_renderer.draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Tetris::createGraphicsPipeline()
{
    const auto vertShaderCode = utils::fs::readFile("./shaders/shader.vert.spv");
    const auto fragShaderCode = utils::fs::readFile("./shaders/shader.frag.spv");
    const auto vertShaderModule = create::shaderModule(*m_device, vertShaderCode);
    const auto fragShaderModule = create::shaderModule(*m_device, fragShaderCode);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = create::pipelineLayoutCreateInfo(
        std::array<VkDescriptorSetLayout, 1>{*m_descriptorSetLayout},
        std::array<VkPushConstantRange, 0>{}
    );

    if (vkCreatePipelineLayout(*m_device, &pipelineLayoutInfo, nullptr, &m_vkPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    m_vkPipeline = defaultGraphicsPipeline(*m_device, m_vkRenderPass,
        m_vkPipelineLayout, vertShaderModule, fragShaderModule);

    vkDestroyShaderModule(*m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(*m_device, vertShaderModule, nullptr);
}
