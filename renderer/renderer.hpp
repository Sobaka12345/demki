#pragma once

#include "model.hpp"
#include "renderable.hpp"

#include <map>
#include <list>

class Renderer
{
public:
    using RenderablePtr = std::weak_ptr<Renderable>;
    using RenderableQueue = std::list<RenderablePtr>;
    using RenderableQueueMap =
        std::map<std::weak_ptr<Model>, RenderableQueue, std::owner_less<std::weak_ptr<Model>>>;
    struct RenderableHandler
    {
    private:
        RenderableQueueMap::iterator mapIter;
        RenderableQueue::iterator queueIter;

        friend class Renderer;
    };

public:
    Renderer();

    virtual void draw(const vk::CommandBuffer& commandBuffer);

    RenderableHandler pushObject(RenderablePtr object);
    void removeObject(RenderableHandler iter);

private:
    RenderableQueueMap m_renderQueues;
};
