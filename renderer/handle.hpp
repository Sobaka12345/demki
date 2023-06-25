#pragma once

#include "assert.hpp"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace vk {

template <typename HandleType>
class Handle
{
protected:
    using VkHandleType = HandleType;

public:
    Handle(HandleType* handlePtr)
        : m_handlePtr(handlePtr ? handlePtr : new HandleType(VK_NULL_HANDLE))
        , m_externalPtr(handlePtr)
        , m_owner(false)
    {
    }

    virtual ~Handle()
    {
        if (!m_externalPtr && m_handlePtr) delete m_handlePtr;
    }

    Handle(Handle&& other) noexcept
        : m_handlePtr(other.m_handlePtr)
        , m_owner(other.m_owner)
        , m_externalPtr(other.m_externalPtr)
    {
        other.m_externalPtr = true;
        other.m_owner = false;
        other.m_handlePtr = nullptr;
    }

    Handle& operator=(Handle&& other) noexcept
    {
        m_handlePtr = other.m_handlePtr;
        m_owner = other.m_owner;
        m_externalPtr = other.m_externalPtr;

        other.m_externalPtr = true;
        other.m_owner = false;
        other.m_handlePtr = nullptr;

        return *this;
    }

    const HandleType* handlePtr() const { return m_handlePtr;  }
    HandleType handle() const { return m_handlePtr ? *m_handlePtr : VK_NULL_HANDLE; }
    operator HandleType() const { return handle(); }

    Handle(const Handle& other) = delete;
    Handle& operator=(const Handle& other) = delete;

    bool owner() const { return m_owner; }
    void setOwner(bool value) { m_owner = value; }

protected:

    auto create(auto createFunc, auto&&... args)
    {
        setOwner(true);
        return createFunc(std::forward<decltype(args)>(args)..., m_handlePtr);
    }

    void destroy(auto destroyFunc, auto&&... args)
    {
        if (owner())
        {
            destroyFunc(std::forward<decltype(args)>(args)...);
        }
    }

private:
    bool m_owner;
    bool m_externalPtr;
    HandleType* m_handlePtr;

    template<typename T>
    friend class HandleVector;
};

template <typename T>
class HandleVector
{
public:
    HandleVector() {}
    ~HandleVector()
    {
        clear();
    }

    HandleVector(HandleVector&& other)
        : m_handles(std::move(other.m_handles))
        , m_vkHandles(std::move(other.m_vkHandles))
    {}

    HandleVector& operator=(HandleVector&& other)
    {
        m_handles = std::move(other.m_handles);
        m_vkHandles = std::move(other.m_vkHandles);
        return *this;
    }

    HandleVector(const HandleVector& other) = delete;
    HandleVector& operator=(const HandleVector& other) = delete;

    template <typename ...Args>
    HandleVector(size_t count, Args&&... constructorArgs)
    {
        m_handles.reserve(count);
        m_vkHandles.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            m_vkHandles[i] = VK_NULL_HANDLE;
            m_handles.emplace_back(std::forward<Args>(constructorArgs)..., &m_vkHandles[i]);
        }
    }

    template <typename ...Args>
    const auto& emplace_back(Args&&... constructorArgs)
    {
        const size_t initialCapacity = m_vkHandles.capacity();

        m_vkHandles.push_back(VK_NULL_HANDLE);
        auto& newEl = m_handles.emplace_back(std::forward<Args>(constructorArgs)...,
            &m_vkHandles[m_vkHandles.size() - 1]);
        //newEl.setOwner(false);

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();

        return newEl;
    }

    template <typename ...Args>
    void resize(size_t size, Args&&... constructorArgs)
    {
        ASSERT(size > m_handles.size(), "NOT IMPLEMENTED");

        const size_t initialCapacity = m_vkHandles.capacity();
        m_handles.reserve(size);
        m_vkHandles.resize(size, VK_NULL_HANDLE);
        for (size_t i = m_handles.size(); i < m_vkHandles.size(); ++i)
        {
            m_handles.emplace_back(std::forward<Args>(constructorArgs)..., &m_vkHandles[i]);
        }

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();
    }

    void reserve(size_t capacity)
    {
        const size_t initialCapacity = m_vkHandles.capacity();
        m_handles.reserve(capacity);
        m_vkHandles.reserve(capacity);

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();
    }

    void clear()
    {
        m_handles.clear();
        m_vkHandles.clear();
    }

    const T& operator[](size_t i) const { return m_handles[i]; }

    size_t size() const { return m_vkHandles.size(); }

    T* data() { return m_handles.data(); }
    typename T::VkHandleType* handleData() { return m_vkHandles.data(); }

private:
    void revalidatePointers()
    {
        for (size_t i = 0; i < m_vkHandles.size(); ++i)
        {
            m_handles[i].m_handlePtr = &m_vkHandles[i];
        }
    }

private:
    std::vector<T> m_handles;
    std::vector<typename T::VkHandleType> m_vkHandles;
};

template <typename T, VkStructureType sTypeArg>
struct VkStruct : public T
{
    constexpr VkStruct(T structValue) noexcept
        : T{ structValue }
    {
        if constexpr (sTypeArg != VK_STRUCTURE_TYPE_MAX_ENUM)
        {
            T::sType = sTypeArg;
        }

        static_assert(sizeof(VkStruct) == sizeof(T));
    }
};

}