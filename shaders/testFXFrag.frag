#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformBufferObject {
    float warpFactor;
} warp;

void main() {
    vec2 newCoords = vec2(pow(texCoords.x, warp.warpFactor), texCoords.y);
    outColor = texture(texSampler, newCoords);
}
