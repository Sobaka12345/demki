#pragma once

#include "vulkan/vulkan_core.h"

#include <cstdlib>

namespace vk { namespace handles {

class Allocator
{
public:
    inline operator VkAllocationCallbacks()
    {
        VkAllocationCallbacks result;

        result.pUserData = static_cast<void*>(this);
        result.pfnAllocation = &allocation;
        result.pfnReallocation = &reallocation;
        result.pfnFree = &free;
        result.pfnInternalAllocation = nullptr;
        result.pfnInternalFree = nullptr;

        return result;
    };

protected:
    ~Allocator(){};

private:
    static void* VKAPI_CALL allocation(
        void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
    {
        return static_cast<Allocator*>(pUserData)->allocation(size, alignment, allocationScope);
    }

    static void* VKAPI_CALL reallocation(void* pUserData,
        void* pOriginal,
        size_t size,
        size_t alignment,
        VkSystemAllocationScope allocationScope)
    {
        return static_cast<Allocator*>(pUserData)->reallocation(
            pOriginal, size, alignment, allocationScope);
    }

    static void VKAPI_CALL free(void* pUserData, void* pMemory)
    {
        return static_cast<Allocator*>(pUserData)->free(pMemory);
    }

    virtual void* allocation(size_t size, size_t alignment, VkSystemAllocationScope)
    {
        return std::aligned_alloc(alignment, size);
    }

    virtual void* reallocation(
        void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope)
    {
        if (auto mod = size % alignment; mod != 0)
        {
            size += alignment - mod;
        }

        return std::realloc(pOriginal, size);
    }

    virtual void free(void* pMemory) { std::free(pMemory); }
};

}}    //  namespace vk::handles
