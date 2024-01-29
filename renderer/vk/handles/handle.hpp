#pragma once

#include "assert.hpp"
#include "vk/utils.hpp"

#include <span>
#include <vector>
#include <unordered_set>

#include <vulkan/vulkan_core.h>

namespace vk { namespace handles {

#define HANDLE(type)           \
    friend class Handle<type>; \
    friend class HandleVector<type>

struct Watcher
{
    virtual void notify() = 0;

protected:
    ~Watcher() {}
};

template <typename HandleType>
class HandlePtr : private Watcher
{
public:
    HandlePtr(HandleType* handlePtr);

    HandlePtr(const HandlePtr& other);
    HandlePtr(HandlePtr&& other);

    template <typename DT>
        requires std::is_base_of_v<HandleType, DT>
    HandlePtr(const HandlePtr<DT>& other);

    ~HandlePtr();

    bool operator==(const HandlePtr& other) const { return m_ptr == other.m_ptr; }

    bool isAlive() const { return m_ptr; }

    bool isDead() const { return !m_ptr; }

    HandleType* operator->() { return m_ptr; }

    const HandleType* operator->() const { return m_ptr; }

    HandleType* ptr() { return m_ptr; }

    const HandleType* ptr() const { return m_ptr; }

    virtual void notify() { m_ptr = nullptr; }

private:
    mutable HandleType* m_ptr;

    template <typename>
    friend class Handle;

    template <typename>
    friend class HandlePtr;
};

template <typename T>
struct HandlePtrHasher
{
    std::size_t operator()(const vk::handles::HandlePtr<T>& s) const noexcept
    {
        return reinterpret_cast<size_t>(s.ptr());
    }
};

template <typename HandleType>
class Handle
{
public:
    using VkHandleType = HandleType;

public:
    explicit Handle(HandleType* handlePtr)
        : m_handlePtr(handlePtr ? handlePtr : new HandleType(VK_NULL_HANDLE))
        , m_externalPtr(handlePtr)
        , m_owner(false)
    {}

    explicit Handle(HandleType handle)
        : Handle(static_cast<HandleType*>(nullptr))
    {
        *m_handlePtr = handle;
    }

    virtual ~Handle()
    {
        if (!m_externalPtr && m_handlePtr) delete m_handlePtr;

        for (auto watcher : m_deleteWatchers)
        {
            watcher->notify();
        }
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

    const HandleType* handlePtr() const { return m_handlePtr; }

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
            setOwner(false);
        }
    }

private:
    void addDeleteWatcher(Watcher* watcherPtr) { m_deleteWatchers.insert(watcherPtr); }

    void removeDeleteWatcher(Watcher* watcherPtr) { m_deleteWatchers.erase(watcherPtr); }

private:
    bool m_owner;
    bool m_externalPtr;
    HandleType* m_handlePtr;
    std::unordered_set<Watcher*> m_deleteWatchers;

    template <typename>
    friend class HandleVector;

    template <typename>
    friend class HandlePtr;
};

template <typename HandleType>
inline HandlePtr<HandleType>::HandlePtr(HandleType* handlePtr)
    : m_ptr(handlePtr)
{
    handlePtr->addDeleteWatcher(this);
}

template <typename HandleType>
inline HandlePtr<HandleType>::HandlePtr(const HandlePtr& other)
    : m_ptr(other.m_ptr)
{
    if (isAlive()) m_ptr->addDeleteWatcher(this);
}

template <typename HandleType>
inline HandlePtr<HandleType>::HandlePtr(HandlePtr&& other)
    : m_ptr(other.m_ptr)
{
    if (isAlive()) m_ptr->addDeleteWatcher(this);
}

template <typename HandleType>
template <typename DT>
    requires std::is_base_of_v<HandleType, DT>
inline HandlePtr<HandleType>::HandlePtr(const HandlePtr<DT>& other)
    : m_ptr(other.m_ptr)
{
    if (isAlive()) m_ptr->addDeleteWatcher(this);
}

template <typename HandleType>
inline HandlePtr<HandleType>::~HandlePtr()
{
    if (isAlive()) m_ptr->removeDeleteWatcher(this);
}

template <typename T>
class HandleVector
{
    friend T;

public:
    HandleVector() {}

    ~HandleVector() { clear(); }

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

    template <typename... Args>
    HandleVector(size_t count, Args&&... constructorArgs)
    {
        m_handles.reserve(count);
        m_vkHandles.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            m_vkHandles[i] = VK_NULL_HANDLE;
            m_handles.emplace_back(
                createObject(std::forward<Args>(constructorArgs)..., &m_vkHandles[i]));
        }
    }

    template <typename... Args>
    auto& emplaceBack(Args&&... constructorArgs)
    {
        const size_t initialCapacity = m_vkHandles.capacity();

        m_vkHandles.push_back(VK_NULL_HANDLE);
        auto& newEl = m_handles.emplace_back(createObject(std::forward<Args>(constructorArgs)...,
            &m_vkHandles[m_vkHandles.size() - 1]));

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();

        return newEl;
    }

    template <typename... Args>
    void resize(size_t size, Args&&... constructorArgs)
    {
        if (size == m_vkHandles.size()) return;

        const size_t initialCapacity = m_vkHandles.capacity();
        m_vkHandles.resize(size, VK_NULL_HANDLE);

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();

        if (m_handles.size() > m_vkHandles.size())
        {
            while (m_handles.size() < m_vkHandles.size()) m_handles.pop_back();
        }
        else
        {
            m_handles.reserve(size);
            for (size_t i = m_handles.size(); i < m_vkHandles.size(); ++i)
            {
                m_handles.emplace_back(
                    createObject(std::forward<Args>(constructorArgs)..., &m_vkHandles[i]));
            }
        }
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

    T& back() { return m_handles.back(); }

    const T& back() const { return m_handles.back(); }

    T& front() { return m_handles.front(); }

    const T& front() const { return m_handles.front(); }

    T& operator[](size_t i) { return m_handles[i]; }

    const T& operator[](size_t i) const { return m_handles[i]; }

    size_t size() const { return m_vkHandles.size(); }

    T* data() { return m_handles.data(); }

    const T* data() const { return m_handles.data(); }

    typename T::VkHandleType* handleData() { return m_vkHandles.data(); }

    const typename T::VkHandleType* handleData() const { return m_vkHandles.data(); }

private:
    template <typename IT>
    constexpr typename std::enable_if<std::is_std_span<std::remove_reference_t<IT>>::value,
        typename std::remove_reference_t<IT>::element_type>::type&
        indexedValueIfSpan(size_t index, std::remove_reference_t<IT>& value) const
    {
        return value[index];
    }

    template <typename IT>
    constexpr typename std::enable_if<!std::is_std_span<std::remove_reference_t<IT>>::value,
        std::remove_reference_t<IT>>::type&&
        indexedValueIfSpan(size_t index, std::remove_reference_t<IT>& value) const
    {
        return static_cast<std::remove_reference_t<IT>&&>(value);
    }

    template <typename CreateFunc, typename FuncArgs, typename ConstructorArgs>
    //  requires IsValidVkCreateFunction<CreateFunc, FuncArgs>
    auto emplaceBackBatch(
        CreateFunc func, size_t size, FuncArgs&& funcArgs, ConstructorArgs&& constructorArgs)
    {
        const size_t initialCapacity = m_vkHandles.capacity();
        const size_t oldSize = m_vkHandles.size();
        m_vkHandles.resize(oldSize + size, VK_NULL_HANDLE);

        ASSERT(
            std::apply([&](auto&&... args) { return func(args..., m_vkHandles.data() + oldSize); },
                std::forward<FuncArgs>(funcArgs)) == VK_SUCCESS,
            "failed to create vk handles");

        if (m_vkHandles.capacity() != initialCapacity) revalidatePointers();

        m_handles.reserve(m_vkHandles.size());
        for (size_t i = m_handles.size(); i < m_vkHandles.size(); ++i)
        {
            std::apply(
                [&](auto&&... args) {
                    m_handles.emplace_back(createObject(
                        indexedValueIfSpan<decltype(args)>(i,
                            (std::forward<decltype(args)>(args)))...,
                        &m_vkHandles[i]));
                },
                constructorArgs);
        }

        return m_handles.begin() + oldSize;
    }

    template <typename... Args>
    auto createObject(Args&&... args)
    {
        return T{ std::forward<Args>(args)... };
    }

    void revalidatePointers()
    {
        for (size_t i = 0; i < m_handles.size(); ++i)
        {
            m_handles[i].m_handlePtr = &m_vkHandles[i];
        }
    }

private:
    std::vector<T> m_handles;
    std::vector<typename T::VkHandleType> m_vkHandles;
};

}}    //  namespace vk::handles
