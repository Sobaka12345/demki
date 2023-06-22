#pragma once

#include <graphical_application.hpp>

class Dummy : public GraphicalApplication
{
public:
    Dummy();
    ~Dummy();

private:
    virtual void initApplication() override;
    virtual void update(int64_t dt) override;
    virtual void recordCommandBuffer(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex) override;

};
