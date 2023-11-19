#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {

	gl_PointSize = 14.0;
	gl_Position = vec4(inPosition.xy, 0.9, 1.0);
	fragColor = inColor;
}
