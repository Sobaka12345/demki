#version 450

#include "shared_types.hpp"

layout(push_constant) uniform block
{
	Globals globals;
};

layout(set = 0, binding = 0) readonly buffer Vertices
{
    Vertex3DColoredTextured vertices[];
};

layout(set = 0, binding = 1) readonly buffer Indices
{
    IndexType indices[];
};

layout(location = 0) out vec3 fragColor;

void main() {
    Vertex3DColoredTextured vertex = vertices[indices[gl_VertexIndex]];
    gl_Position = vec4(vertex.pos, 1.0);
    fragColor = vertex.color;
}
