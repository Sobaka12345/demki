#pragma once

#include <memory>

namespace vk {
class CommandBuffer;
}

class Model;

class IRenderable
{
public:
    virtual ~IRenderable() {};
    virtual void draw(const vk::CommandBuffer& commandBuffer) const = 0;

    virtual std::weak_ptr<Model> model() const = 0;
    virtual void setModel(std::weak_ptr<Model> model) = 0;
};
