#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformBufferObject {
    float warpFactor;
    float xPadding;
    float yPadding;
} warp;

void main() {
    vec2 newCoords = texCoords;

    if(newCoords.x < warp.xPadding || newCoords.x > 1.0 - warp.xPadding || newCoords.y < warp.yPadding || newCoords.y > 1.0 - warp.yPadding)
    {
        outColor = vec4(0,0,0,0);
    }
    else
    {
        newCoords = vec2(pow(texCoords.x, warp.warpFactor), texCoords.y);
        outColor = texture(texSampler, newCoords);
    }
}
