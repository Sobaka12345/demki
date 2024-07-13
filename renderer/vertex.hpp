#pragma once

#define GLM_EXT_INCLUDED
#ifndef GLM_ENABLE_EXPERIMENTAL
#	define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtx/hash.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>

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
