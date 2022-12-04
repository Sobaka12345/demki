#version 450

layout(binding = 0) uniform UBOModel {
    mat4 model;
} model;

layout(binding = 1) uniform UBOViewProj {
    mat4 view;
    mat4 proj;
} viewProj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = viewProj.proj * viewProj.view * model.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
