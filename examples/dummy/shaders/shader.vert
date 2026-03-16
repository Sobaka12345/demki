#version 450

#include "shared_types.hpp"

// layout(location = 0) in vec2 inPosition;
// layout(location = 1) in vec2 velocity;
// layout(location = 2) in vec4 inColor;


layout(binding = 0) readonly buffer Indices
{
	IndexType indices[];
};

layout(binding = 1) readonly buffer Vertices
{
	Vertex3DColoredTextured vertices[];
};

layout(binding = 2) readonly buffer DrawCommands
{
	DrawCommand drawCommands[];
};

layout(location = 0) out vec4 fragColor;

void main() {
	DrawCommand c = drawCommands[gl_VertexIndex];

	Vertex3DColoredTextured vt = vertices[indices[gl_VertexIndex]];
	gl_Position = vec4(vt.pos, 1) /** c.position * c.orientation*/;
	fragColor = vec4(vt.color, 1);
}
