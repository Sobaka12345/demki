#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define CREATE_INFO_PROPERTY(type, name, default) \
                                              \
public:                                       \
    CreateInfo& name(type value)              \
    {                                         \
        m_##name = value;                     \
        return *this;                         \
    }                                         \
    const auto& name() const                  \
    {                                         \
        return m_##name;                      \
    }                                         \
    auto& name()                              \
    {                                         \
        return m_##name;                      \
    }                                         \
                                              \
private:                                      \
    type m_##name = default;


#include <assert.hpp>

#include <cstdint>
#include <type_traits>

template <typename EnumT>
EnumT& operator++(EnumT& e) {
    using IntType = typename std::underlying_type<EnumT>::type;
    e = static_cast<EnumT>( static_cast<IntType>(e) + 1 );
    if (e == EnumT::COUNT) e = EnumT::BEGIN;
    return e;
}

template<typename E>
constexpr auto enumT(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

