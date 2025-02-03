#pragma once

#include <span>
#include <vector>

namespace renderer {

class IPool {
public:
    virtual ~IPool() {};
};

template <typename BaseT>
    requires std::is_base_of_v<IPool, BaseT>
class IPoolT : public BaseT
{
public:
    explicit IPoolT(size_t capacity = 1) noexcept {
        m_objects.reserve(capacity);
    }

    [[nodiscard]] inline std::span<typename BaseT::ElementType> spawn(size_t count = 1) noexcept {
        if (m_objects.size() == m_objects.capacity()) {
            return std::span<typename BaseT::ElementType>{};
        }

        return spawnImpl(count);
    }

    inline typename BaseT::ElementType& at(size_t index) noexcept { m_objects.at(index); }
    inline void swap(size_t lhs, size_t rhs)  noexcept { std::swap(m_objects[lhs], m_objects[rhs]); }

protected:
    [[nodiscard]] virtual std::span<typename BaseT::ElementType> spawnImpl(size_t count = 1) noexcept = 0;

private:
    std::vector<typename BaseT::ElementType> m_objects;
};

}
