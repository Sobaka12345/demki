#include "tetris.hpp"

#include "field.hpp"

#include <model.hpp>

#include <vk/descriptor_set.hpp>
#include <vk/descriptor_set_layout.hpp>
#include <vk/vertex.hpp>
#include <vk/creators.hpp>
#include <vk/pipeline_layout.hpp>
#include <vk/render_pass.hpp>
#include <vk/sampler.hpp>
#include <vk/shader_module.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

using namespace vk;

class TetrisDSL : public DescriptorSetLayout
{
public:
    static constexpr create::DescriptorSetLayoutBinding s_modelBinding =
        create::DescriptorSetLayoutBinding{}
            .descriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
            .descriptorCount(1)
            .stageFlags(VK_SHADER_STAGE_VERTEX_BIT)
            .binding(0);

    static constexpr auto s_cameraBinding =
        create::DescriptorSetLayoutBinding{}
            .descriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .descriptorCount(1)
            .stageFlags(VK_SHADER_STAGE_VERTEX_BIT)
            .binding(1);

    static constexpr auto s_samplerBinding =
        create::DescriptorSetLayoutBinding{}
            .descriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            .descriptorCount(1)
            .stageFlags(VK_SHADER_STAGE_FRAGMENT_BIT)
            .binding(2);

    static constexpr std::array s_layoutBindings = { s_modelBinding, s_cameraBinding,
        s_samplerBinding };

public:
    TetrisDSL(const Device& device)
        : DescriptorSetLayout(device,
              DescriptorSetLayoutCreateInfo()
                  .pBindings(s_layoutBindings.data())
                  .bindingCount(s_layoutBindings.size()))
    {}
};

class TetrisDP : public DescriptorPool
{
public:
    TetrisDP(const Device& device)
        : DescriptorPool(device,
              3,
              std::array{
                  create::descriptorPoolSize(TetrisDSL::s_modelBinding.descriptorType(), 1),
                  create::descriptorPoolSize(TetrisDSL::s_cameraBinding.descriptorType(), 1),
                  create::descriptorPoolSize(TetrisDSL::s_samplerBinding.descriptorType(), 1),
              })
    {}
};

class TetrisCamera : public UBOValue<UBOViewProjection>
{
public:
    TetrisCamera(const DescriptorSet* descriptorSet, std::shared_ptr<UBOHandler> handler)
        : UBOValue<UBOViewProjection>(descriptorSet, handler)
    {}
};

static constexpr std::array<Vertex3DColoredTextured, 8> s_cubeVertices = {
    Vertex3DColoredTextured{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    Vertex3DColoredTextured{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    Vertex3DColoredTextured{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
    Vertex3DColoredTextured{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    Vertex3DColoredTextured{ { 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
};

static constexpr std::array<uint32_t, 36> s_cubeIndices = { 7, 6, 2, 2, 3, 7, 0, 4, 5, 5, 1, 0, 0,
    2, 6, 6, 4, 0, 7, 3, 1, 1, 5, 7, 3, 2, 0, 0, 1, 3, 4, 6, 7, 7, 5, 4 };

Tetris::Tetris()
    : m_dx(0)
    , m_rotate(false)
    , m_flushedTotal(0)
{
    m_gameTimer.setIntervalMS(500);
    m_moveTimer.setIntervalMS(150);
    m_rotationTimer.setIntervalMS(200);
}

Tetris::~Tetris() {}

void Tetris::initTextures()
{
    int texWidth, texHeight, texChannels;
    auto imagePath = (utils::fs::s_executablePath / "textures" / "roshi.jpg").string();
    stbi_uc* pixels =
        stbi_load(imagePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    ASSERT(pixels, "failed to load texture image!");

    Buffer stagingBuffer(*m_device, Buffer::staging().size(imageSize));
    stagingBuffer
        .allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        .lock()
        ->map()
        .lock()
        ->write(pixels, imageSize);

    m_roshiImage = std::make_unique<vk::Image>(*m_device,
        ImageCreateInfo()
            .imageType(VK_IMAGE_TYPE_2D)
            .extent({ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 })
            .mipLevels(1)
            .arrayLayers(1)
            .format(VK_FORMAT_R8G8B8A8_SRGB)
            .tiling(VK_IMAGE_TILING_OPTIMAL)
            .initialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
            .usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .samples(VK_SAMPLE_COUNT_1_BIT)
            .sharingMode(VK_SHARING_MODE_EXCLUSIVE));
    m_roshiImage->allocateAndBindMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_roshiImage->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const auto copyRegion = create::bufferImageCopy(0,
        0,
        0,
        create::imageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1),
        { 0, 0, 0 },
        { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 });

    stagingBuffer.copyToImage(*m_roshiImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegion);

    m_roshiImage->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stbi_image_free(pixels);

    m_roshiImageView = std::make_unique<ImageView>(*m_device,
        defaultImageView(*m_roshiImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT));

    m_roshiImageSampler = std::make_unique<Sampler>(*m_device,
        SamplerCreateInfo()
            .magFilter(VK_FILTER_LINEAR)
            .minFilter(VK_FILTER_LINEAR)
            .mipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
            .addressModeU(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .addressModeV(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .addressModeW(VK_SAMPLER_ADDRESS_MODE_REPEAT)
            .anisotropyEnable(VK_FALSE)
            .maxAnisotropy(m_device->physicalDeviceProperties().limits.maxSamplerAnisotropy)
            .compareEnable(VK_FALSE)
            .compareOp(VK_COMPARE_OP_ALWAYS)
            .borderColor(VK_BORDER_COLOR_INT_OPAQUE_BLACK)
            .unnormalizedCoordinates(VK_FALSE));
}

void Tetris::initApplication()
{
    glfwSetKeyCallback(m_window, onKeyPressed);

    m_descriptorSetLayout = std::make_unique<TetrisDSL>(*m_device);

    //  Render Pipeline
    createGraphicsPipeline();

    m_cube = resources().loadModel(s_cubeVertices, s_cubeIndices);

    initTextures();

    m_modelBuffer = std::make_unique<UniformBuffer<UBOModel>>(*m_device, Field::s_objectsCount);
    m_modelBuffer->allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).lock()->map();

    m_viewProjectionBuffer = std::make_unique<UniformBuffer<UBOViewProjection>>(*m_device, 1);
    m_viewProjectionBuffer
        ->allocateAndBindMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        .lock()
        ->map();

    m_descriptorPool = std::make_unique<TetrisDP>(*m_device);

    VkDescriptorImageInfo imageInfo{
        .sampler = *m_roshiImageSampler,
        .imageView = *m_roshiImageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    m_descriptorSet = m_descriptorPool->allocateSet(*m_descriptorSetLayout, *m_pipelineLayout);
    const std::array writeSets{
        DescriptorSet::Write{ .bufferInfo = m_modelBuffer->descriptorBufferInfo(),
            .layoutBinding = TetrisDSL::s_modelBinding },
        DescriptorSet::Write{ .bufferInfo = m_viewProjectionBuffer->descriptorBufferInfo(),
            .layoutBinding = TetrisDSL::s_cameraBinding },
        DescriptorSet::Write{ .imageInfo = imageInfo, .layoutBinding = TetrisDSL::s_samplerBinding }
    };
    m_descriptorSet->write(writeSets);

    //  TO DO: MOVE TO CAMERA CLASS
    {
        m_camera = std::make_unique<TetrisCamera>(m_descriptorSet.get(),
            m_viewProjectionBuffer->tryGetUBOHandler());

        constexpr float fov = 45.0f;
        constexpr float distance = Field::s_height * 90.0f / fov;
        UBOViewProjection viewProjection;
        viewProjection.view = glm::lookAt(
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, distance),
            glm::vec3(1.0f * Field::s_width / 2.0f, 1.0f * Field::s_height / 2.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        viewProjection.projection = glm::perspective(glm::radians(fov),
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
        while (thisPtr->m_field->tryMoveFigure(0, 1))
            ;
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
            std::cout << "FLUSHED ROWS COUNT: " << flushed << " TOTAL: " << m_flushedTotal
                      << std::endl;

            if (flushed && m_flushedTotal % 8 == 0)
            {
                m_gameTimer.setInterval(m_gameTimer.interval() * 0.8f);
            }
        }
    }
}

void Tetris::recordCommandBuffer(const vk::CommandBuffer& commandBuffer,
    const vk::Framebuffer& framebuffer)
{
    const std::array<VkClearValue, 2> clearValues{ VkClearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } },
        VkClearValue{ { 1.0f, 0 } } };

    const auto renderPassInfo =
        create::RenderPassBeginInfo{}
            .renderPass(*m_renderPass)
            .framebuffer(framebuffer)
            .renderArea(VkRect2D{ VkOffset2D{ 0, 0 },
                VkExtent2D{ m_vkSwapChainExtent.width, m_vkSwapChainExtent.height } })
            .clearValueCount(clearValues.size())
            .pClearValues(clearValues.data());

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vkSwapChainExtent.width);
    viewport.height = static_cast<float>(m_vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent.width = m_vkSwapChainExtent.width;
    scissor.extent.height = m_vkSwapChainExtent.height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    m_renderer.draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
}

void Tetris::createGraphicsPipeline()
{
    const ShaderModule vertShaderModule(*m_device, "./shaders/shader.vert.spv");
    const ShaderModule fragShaderModule(*m_device, "./shaders/shader.frag.spv");

    const std::array shaderStageCreateInfos = {
        create::PipelineShaderStageCreateInfo()
            .stage(VK_SHADER_STAGE_VERTEX_BIT)
            .module(vertShaderModule)
            .pName("main"),
        create::PipelineShaderStageCreateInfo()
            .stage(VK_SHADER_STAGE_FRAGMENT_BIT)
            .module(fragShaderModule)
            .pName("main"),
    };

    const auto bindingDescriptions = Vertex3DColoredTextured::bindingDescription();
    const auto attrubuteDescriptions = Vertex3DColoredTextured::attributeDescriptions();

    const auto vertexInput =
        create::PipelineVertexInputStateCreateInfo()
            .vertexBindingDescriptionCount(bindingDescriptions.size())
            .pVertexBindingDescriptions(bindingDescriptions.data())
            .vertexAttributeDescriptionCount(attrubuteDescriptions.size())
            .pVertexAttributeDescriptions(attrubuteDescriptions.data());

    m_pipelineLayout = std::make_unique<PipelineLayout>(*m_device,
        PipelineLayoutCreateInfo()
            .pSetLayouts(m_descriptorSetLayout->handlePtr())
            .setLayoutCount(1));

    m_pipeline = std::make_unique<GraphicsPipeline>(*m_device,
        VK_NULL_HANDLE,
        defaultGraphicsPipeline()
            .renderPass(*m_renderPass)
            .layout(*m_pipelineLayout)
            .stageCount(shaderStageCreateInfos.size())
            .pStages(shaderStageCreateInfos.data())
            .pVertexInputState(&vertexInput));
}
