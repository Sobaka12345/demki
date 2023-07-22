#include "renderable_pack.hpp"

#include <imodel.hpp>
#include <irenderable.hpp>

void RenderablePack::draw(const RenderContext& context) const
{
    for (auto modelQueueIter = m_renderQueues.begin(); modelQueueIter != m_renderQueues.end();)
    {
        if (!modelQueueIter->first.expired())
        {
            const auto modelPtr = modelQueueIter->first.lock();
            modelPtr->bind(context);

            for (auto iter = modelQueueIter->second.begin(); iter != modelQueueIter->second.end();)
            {
                if (!iter->expired())
                {
                    iter->lock()->draw(context);
                    ++iter;
                }
                else
                {
                    iter = modelQueueIter->second.erase(iter);
                }
            }
            ++modelQueueIter;
        }
        else
        {
            modelQueueIter = m_renderQueues.erase(modelQueueIter);
        }
    }
}

RenderablePack::RenderableHandler RenderablePack::pushObject(std::weak_ptr<IRenderable> object)
{
    auto mapIter = m_renderQueues.find(object.lock()->model());
    if (mapIter == m_renderQueues.end())
    {
        mapIter =
            m_renderQueues.emplace(std::pair{ object.lock()->model(), RenderableQueue{} }).first;
    }
    auto queueIter = mapIter->second.insert(mapIter->second.cend(), object);

    RenderableHandler handler;
    handler.queueIter = queueIter;
    handler.mapIter = mapIter;
    return handler;
}

void RenderablePack::removeObject(RenderableHandler handler)
{
    handler.mapIter->second.erase(handler.queueIter);
}
