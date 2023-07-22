#pragma once

#include "irenderable.hpp"

#include <list>
#include <map>
#include <memory>

class IModel;
class IRenderable;
class RenderContext;

class RenderablePack
{
    using RenderableQueue = std::list<std::weak_ptr<IRenderable>>;
    using RenderableQueueMap =
        std::map<std::weak_ptr<IModel>, RenderableQueue, std::owner_less<std::weak_ptr<IModel>>>;

    struct RenderableHandler
    {
    private:
        RenderableQueueMap::iterator mapIter;
        RenderableQueue::iterator queueIter;

        friend class RenderablePack;
    };

public:
    RenderablePack() = default;
    void draw(const RenderContext& context) const;
    RenderableHandler pushObject(std::weak_ptr<IRenderable> object);
    void removeObject(RenderableHandler iter);

private:
    mutable RenderableQueueMap m_renderQueues;
};
