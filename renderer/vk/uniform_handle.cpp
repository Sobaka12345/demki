#include "uniform_handle.hpp"

#include "handles/memory.hpp"

#include <render_context.hpp>

namespace vk {

UniformHandle::UniformHandle() {}

UniformHandle::~UniformHandle() {}

void UniformHandle::write(const void* src, uint32_t layoutSize)
{
    if (!memory.expired())
    {
        memory.lock()->mapped->write(src, layoutSize, offset);
    }
}

void UniformHandle::sync(uint32_t layoutSize)
{
    if (!memory.expired())
    {
        memory.lock()->mapped->sync(layoutSize, offset);
    }
}

uint32_t UniformHandle::resourceOffset() const
{
    return offset;
}

uint64_t UniformHandle::resource() const
{
    return resourceId;
}

std::shared_ptr<UniformHandle> UniformHandle::create()
{
    return std::shared_ptr<UniformHandle>{new UniformHandle};
}

}    //  namespace vk
