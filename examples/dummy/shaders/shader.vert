#version 450

#include "../glsl_types.hpp"

// layout(location = 0) in vec2 inPosition;
// layout(location = 1) in vec2 velocity;
// layout(location = 2) in vec4 inColor;

layout(binding = 0) readonly buffer DrawCommands
{
	Draw drawCommands[];
};

layout(location = 0) out vec4 fragColor;

void main() {

    // gl_Position = camera.projection * camera.view * position.model * vec4(inPosition, 1.0);
     fragColor = vec4(1,1,1,1);
}
