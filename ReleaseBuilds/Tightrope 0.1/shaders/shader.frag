#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler[12];


layout(location = 0) in vec2 texCoords;
layout(location = 1) in float texID;
layout(location = 2) in vec2 minUV;
layout(location = 3) in vec2 maxUV;
layout(location = 4) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 finalTex = texCoords * (maxUV - minUV) + minUV;
    outColor = texture(texSampler[int(round(texID))], finalTex) * inColor;
}
