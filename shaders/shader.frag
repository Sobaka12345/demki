#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexture;

layout(binding = 2) uniform sampler2D textureSampler;

void main() {
	//outColor = vec4(fragTexture, 0.0, 1.0);
	outColor = texture(textureSampler, fragTexture);
}
