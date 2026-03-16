#ifndef SHARED_TYPES
#define SHARED_TYPES

#ifdef GLSL_HOST
    #include <glm/ext/scalar_uint_sized.hpp>
    #include <glm/ext/matrix_float4x4.hpp>
    #include <glm/ext/vector_float2.hpp>
    #include <glm/ext/vector_float3.hpp>
    #include <glm/ext/vector_float4.hpp>
    using uint = glm::uint32;
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat4x4 = glm::mat4x4;
    using IndexType = uint;
    #define STRUCT(T) struct alignas(16) T
#else
#   define alignas(T)
#   define IndexType uint
#   define STRUCT(T) struct alignas(16) T
#endif

STRUCT(Vertex3D)
{
    alignas(16) vec3 pos;

#ifdef GLSL_HOST
    bool operator==(const Vertex3D& other) const
    {
        return pos == other.pos;
    }
#endif
};

STRUCT(Vertex3DColored)
{
    alignas(16) vec3 pos;
    alignas(16) vec3 color;

#ifdef GLSL_HOST
    bool operator==(const Vertex3DColored& other) const
    {
        return pos == other.pos && color == other.color;
    }
#endif
};

STRUCT(Vertex3DColoredTextured)
{
    alignas(16) vec3 pos;
    alignas(16) vec3 color;
    alignas(16) vec2 texture;

#ifdef GLSL_HOST
    bool operator==(const Vertex3DColoredTextured& other) const
    {
        return pos == other.pos && color == other.color && texture == other.texture;
    }
#endif
};

STRUCT(DrawCommand)
{
    alignas(16) vec4 position;
    alignas(16) vec4 orientation;
    float scale;
};

#ifdef GLSL_HOST

#include <glm/gtx/hash.hpp>

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

#endif // GLSL_HOST

#endif // SHARED_TYPES
