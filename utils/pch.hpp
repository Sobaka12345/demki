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

#include <algorithm>
#include <cstdint>
#include <type_traits>

template <typename EnumT>
EnumT& operator++(EnumT& e) {
    using IntType = typename std::underlying_type<EnumT>::type;
    e = static_cast<EnumT>( static_cast<IntType>(e) + 1 );
    if (e > EnumT::COUNT) e = EnumT::BEGIN;
    return e;
}

template<typename E>
constexpr auto enumT(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

template <typename EnumT>
EnumT operator&(EnumT lhs, EnumT rhs) {
    return static_cast<EnumT>(enumT(lhs) & enumT(rhs));
}

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};


struct Vertex3D
{
    glm::vec3 pos;

    bool operator==(const Vertex3D& other) const { return pos == other.pos; }
};

struct Vertex3DColored
{
    glm::vec3 pos;
    glm::vec3 color;

    bool operator==(const Vertex3DColored& other) const
    {
        return pos == other.pos && color == other.color;
    }
};

struct Vertex3DColoredTextured
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texture;

    bool operator==(const Vertex3DColoredTextured& other) const
    {
        return pos == other.pos && color == other.color && texture == other.texture;
    }
};

namespace std {

template <>
struct hash<Vertex3D>
{
    size_t operator()(Vertex3D const& vertex) const { return hash<glm::vec3>()(vertex.pos); }
};

template <>
struct hash<Vertex3DColored>
{
    size_t operator()(Vertex3DColored const& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
    }
};

template <>
struct hash<Vertex3DColoredTextured>
{
    size_t operator()(Vertex3DColoredTextured const& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texture) << 1);
    }
};

}    //  namespace std
