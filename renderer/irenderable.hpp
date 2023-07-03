#pragma once

#include <memory>

namespace vk { namespace handles {
class CommandBuffer;
}}

class Model;

class IRenderable
{
public:
    virtual ~IRenderable(){};
    virtual void draw(const vk::handles::CommandBuffer& commandBuffer) const = 0;

    virtual std::weak_ptr<Model> model() const = 0;
    virtual void setModel(std::weak_ptr<Model> model) = 0;
};
