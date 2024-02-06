#pragma once

#include <ishader_interface_handle.hpp>
#include <operation_context.hpp>

#include <memory>

template <typename LayoutType>
class UniformValue
{
public:
    constexpr static uint64_t s_layoutSize = sizeof(LayoutType);

public:
    UniformValue(std::shared_ptr<IShaderInterfaceHandle> handler)
        : m_uniformHandle(handler)
    {}

    inline void set(LayoutType value) { m_uniformHandle->write<LayoutType>(&value); }

    inline const LayoutType& get() const { return *m_uniformHandle->read<LayoutType>(); }

    inline const std::weak_ptr<IShaderInterfaceHandle> handle() const { return m_uniformHandle; }

private:
    std::shared_ptr<IShaderInterfaceHandle> m_uniformHandle;
};
