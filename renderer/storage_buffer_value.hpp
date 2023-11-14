#pragma once

#include <ishader_interface_handle.hpp>

#include <cstdint>
#include <vector>

template <typename T, size_t Size>
class StorageBufferValue
{
public:
    constexpr static uint64_t s_layoutSize = sizeof(T) * Size;
    constexpr static uint64_t s_elSize = sizeof(T);
    constexpr static uint64_t s_elCount = Size;

public:
    StorageBufferValue(std::shared_ptr<IShaderInterfaceHandle> handler)
        : m_shaderInterfaceHandle(handler)
    {}

    inline void set(std::span<T, Size> value)
    {
        std::copy(value.begin(), value.end(), m_value);
        m_shaderInterfaceHandle->write(m_value.data(), s_layoutSize);
    }

    inline std::span<T, Size> get() const { return m_value; }

    inline const std::weak_ptr<IShaderInterfaceHandle> handle() const
    {
        return m_shaderInterfaceHandle;
    }

private:
    std::array<T, Size> m_value;
    std::shared_ptr<IShaderInterfaceHandle> m_shaderInterfaceHandle;
};
