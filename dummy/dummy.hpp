#pragma once

#include <vk/graphical_application.hpp>

class Dummy : public vk::GraphicalApplication
{
public:
    Dummy();
    ~Dummy();

private:
    virtual void initApplication() override;
    virtual void update(int64_t dt) override;
    virtual void recordCommandBuffer(const vk::CommandBuffer& commandBuffer, 
        const vk::Framebuffer& framebuffer) override;
};
