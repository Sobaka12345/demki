#version 450

layout(set = 0, binding = 0) uniform UBOViewProjection {
    mat4 view;
    mat4 projection;
} camera;

layout(set = 1, binding = 1) uniform UBOModel {
    mat4 model;
} position;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexture;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexture;

void main() {
	gl_Position = camera.projection * camera.view * position.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexture = inTexture;
}
