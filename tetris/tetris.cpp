#include "tetris.hpp"

#include "../renderer/buffer.hpp"
#include "../renderer/vertex.hpp"
#include "../renderer/creators.hpp"

using namespace vk;

template<typename LayoutType>
struct UBObject
{
    LayoutType layout;
    static constexpr uint32_t size = sizeof(LayoutType);
};

struct UBOPersistentCamera
{
    glm::mat4 view;
    glm::mat4 projection;
} s_viewProj;

struct UBOPosition
{
    glm::mat4 model;
};

struct Position
{
    std::shared_ptr<Buffer> position;
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

static constexpr auto s_objectCount = 50;

Tetris::~Tetris()
{
    vkDestroyBuffer(m_vkLogicalDevice, m_uniformBuffers.m_viewProjectionBuffer.buffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, m_uniformBuffers.m_viewProjectionBuffer.memory, nullptr);
    vkDestroyBuffer(m_vkLogicalDevice, m_uniformBuffers.m_modelBufffer.buffer, nullptr);
    vkFreeMemory(m_vkLogicalDevice, m_uniformBuffers.m_modelBufffer.memory, nullptr);
    std::free(m_modelBuffers);

    vkDestroyDescriptorPool(m_vkLogicalDevice, m_vkDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_vkLogicalDevice, m_vkDescriptorSetLayout, nullptr);

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    vkDestroyPipeline(m_vkLogicalDevice, m_vkPipeline, nullptr);
    vkDestroyPipelineLayout(m_vkLogicalDevice, m_vkPipelineLayout, nullptr);
}

void Tetris::initApplication()
{
    // Render Pipeline
    createGraphicsPipeline();

    // Resources
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

void Tetris::createGraphicsPipeline()
{
    const VkDescriptorSetLayoutCreateInfo layoutInfo = create::descriptorSetLayoutCreateInfo(
        std::array<VkDescriptorSetLayoutBinding, 2> {
            create::setLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 0),
            create::setLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
        }
    );

    if (vkCreateDescriptorSetLayout(m_vkLogicalDevice, &layoutInfo, nullptr, &m_vkDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    const auto vertShaderCode = utils::fs::readFile("./shaders/shader.vert.spv");
    const auto fragShaderCode = utils::fs::readFile("./shaders/shader.frag.spv");
    const auto vertShaderModule = create::shaderModule(m_vkLogicalDevice, vertShaderCode);
    const auto fragShaderModule = create::shaderModule(m_vkLogicalDevice, fragShaderCode);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = create::pipelineLayoutCreateInfo(
        std::array<VkDescriptorSetLayout, 1>{m_vkDescriptorSetLayout},
        std::array<VkPushConstantRange, 0>{}
    );

    if (vkCreatePipelineLayout(m_vkLogicalDevice, &pipelineLayoutInfo, nullptr, &m_vkPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    m_vkPipeline = defaultGraphicsPipeline(m_vkLogicalDevice, m_vkRenderPass,
        m_vkPipelineLayout, vertShaderModule, fragShaderModule);

    vkDestroyShaderModule(m_vkLogicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_vkLogicalDevice, vertShaderModule, nullptr);
}


void Tetris::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(s_cubeVertices[0]) * s_cubeVertices.size();

    Buffer stagingBuffer(m_vkLogicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
    stagingBuffer.allocateAndBindMemory(m_vkPhysicalDevice,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto mappedMemory = stagingBuffer.mapMemory();
    mappedMemory.write(s_cubeVertices.data(), bufferSize);
    stagingBuffer.unmapMemory();

    m_vertexBuffer = std::make_unique<Buffer>(
        m_vkLogicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE);
    m_vertexBuffer->allocateAndBindMemory(m_vkPhysicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    stagingBuffer.copyTo(*m_vertexBuffer, m_vkCommandPool, m_vkGraphicsQueue, create::bufferCopy(bufferSize));
}

void Tetris::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(s_cubeIndices[0]) * s_cubeIndices.size();

    Buffer stagingBuffer(m_vkLogicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
    stagingBuffer.allocateAndBindMemory(m_vkPhysicalDevice,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto mappedMemory = stagingBuffer.mapMemory();
    mappedMemory.write(s_cubeIndices.data(), bufferSize);
    stagingBuffer.unmapMemory();

    m_indexBuffer = std::make_unique<Buffer>(
        m_vkLogicalDevice, bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE);
    m_indexBuffer->allocateAndBindMemory(m_vkPhysicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    stagingBuffer.copyTo(*m_indexBuffer, m_vkCommandPool, m_vkGraphicsQueue, create::bufferCopy(bufferSize));
}

void Tetris::createUniformBuffers()
{
    const auto minAlignment = m_vkPhysicalDeviceLimits.minUniformBufferOffsetAlignment;
    m_dynamicAlignment = sizeof(glm::mat4x4);
    if (minAlignment > 0)
    {
        m_dynamicAlignment = (m_dynamicAlignment + minAlignment - 1) & ~(minAlignment - 1);
    }

    VkDeviceSize modelSize = m_dynamicAlignment * s_objectCount;
    VkDeviceSize viewProjSize = sizeof(UBOPersistentCamera);

    m_modelBuffers = reinterpret_cast<glm::mat4x4*>(std::aligned_alloc(m_dynamicAlignment, s_objectCount * m_dynamicAlignment));

    Buffer kek(m_vkLogicalDevice, modelSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    kek.allocateAndBindMemory(m_vkPhysicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    kek.mapMemory();

    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, modelSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        m_uniformBuffers.m_modelBufffer.buffer, m_uniformBuffers.m_modelBufffer.memory);
    vkMapMemory(m_vkLogicalDevice, m_uniformBuffers.m_modelBufffer.memory, 0,
        modelSize, 0, &m_uniformBuffers.m_modelBufffer.mapped);
    m_uniformBuffers.m_modelBufffer.descriptor =
        create::descriptorBufferInfo(m_uniformBuffers.m_modelBufffer.buffer, 0, m_dynamicAlignment);

    create::buffer(m_vkLogicalDevice, m_vkPhysicalDevice, viewProjSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_uniformBuffers.m_viewProjectionBuffer.buffer, m_uniformBuffers.m_viewProjectionBuffer.memory);
    vkMapMemory(m_vkLogicalDevice, m_uniformBuffers.m_viewProjectionBuffer.memory, 0,
        viewProjSize, 0, &m_uniformBuffers.m_viewProjectionBuffer.mapped);
    m_uniformBuffers.m_viewProjectionBuffer.descriptor =
        create::descriptorBufferInfo(m_uniformBuffers.m_viewProjectionBuffer.buffer, 0, sizeof(UBOPersistentCamera));

    updateUniformBuffer(1);
    updateDynUniformBuffer(1);
}

void Tetris::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{
        create::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2),
        create::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_maxFramesInFlight * 2;

    if (vkCreateDescriptorPool(m_vkLogicalDevice, &poolInfo, nullptr, &m_vkDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Tetris::createDescriptorSets()
{
    const auto allocInfo =
        create::descriptorSetAllocateInfo(m_vkDescriptorPool, &m_vkDescriptorSetLayout, 1);

    if (vkAllocateDescriptorSets(m_vkLogicalDevice, &allocInfo, &m_vkDescriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    std::array<VkWriteDescriptorSet, 2> writes = {
        create::writeDescriptorSet(m_vkDescriptorSet, 0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &m_uniformBuffers.m_modelBufffer.descriptor),
        create::writeDescriptorSet(m_vkDescriptorSet, 1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &m_uniformBuffers.m_viewProjectionBuffer.descriptor),
    };
    vkUpdateDescriptorSets(m_vkLogicalDevice, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void Tetris::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

     s_viewProj.view = glm::lookAt(
         glm::vec3(3.0f, 3.0f, 3.0f),
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(0.0f, 0.0f, 1.0f));
     s_viewProj.projection = glm::perspective(
         glm::radians(90.0f),
         m_vkSwapChainExtent.width / static_cast<float>(m_vkSwapChainExtent.height),
         0.1f,
         20.0f);
     s_viewProj.projection[1][1] *= -1;
     memcpy(m_uniformBuffers.m_viewProjectionBuffer.mapped, &s_viewProj, sizeof(UBOPersistentCamera));
}

void Tetris::updateDynUniformBuffer(uint32_t currentImage)
{
    for(uint32_t i = 0; i < s_objectCount; ++i)
    {
        glm::mat4x4* model = reinterpret_cast<glm::mat4x4*>(reinterpret_cast<uint64_t>(m_modelBuffers) + i * m_dynamicAlignment);

        *model = glm::translate(glm::mat4x4(1.0f), -glm::vec3(0.0f, i * 1.0f, 0.0f));
        *model = glm::rotate(
            *model,
            glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));
    }
    memcpy(m_uniformBuffers.m_modelBufffer.mapped, m_modelBuffers, m_dynamicAlignment * s_objectCount);
    VkMappedMemoryRange memoryRange{};
    memoryRange.memory = m_uniformBuffers.m_modelBufffer.memory;
    memoryRange.size = m_dynamicAlignment * s_objectCount;
    vkFlushMappedMemoryRanges(m_vkLogicalDevice, 1, &memoryRange);
}

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

    VkBuffer vertexBuffers[] = {m_vertexBuffer->buffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->buffer(), 0, VK_INDEX_TYPE_UINT16);
    for(uint32_t i = 0; i < s_objectCount; ++i)
    {
        uint32_t offset = i * m_dynamicAlignment;
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_vkPipelineLayout, 0, 1, &m_vkDescriptorSet, 1, &offset);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(s_cubeIndices.size()), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}
