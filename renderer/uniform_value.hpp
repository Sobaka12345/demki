#pragma once

#include <iuniform_handle.hpp>
#include <render_context.hpp>

#include <memory>

template <typename LayoutType>
class UniformValue
{
public:
    constexpr static uint64_t s_layoutSize = sizeof(LayoutType);

public:
    UniformValue(std::shared_ptr<IUniformHandle> handler)
        : m_uniformHandle(handler)
    {}

    inline void set(LayoutType value)
    {
        m_value = std::move(value);
        m_uniformHandle->write(&m_value);
    }

    inline LayoutType get() const { return m_value; }

    inline void sync() { m_uniformHandle->sync<LayoutType>(); }

    inline const std::weak_ptr<IUniformHandle> handle() const { return m_uniformHandle; }

private:
    LayoutType m_value;
    std::shared_ptr<IUniformHandle> m_uniformHandle;
};
