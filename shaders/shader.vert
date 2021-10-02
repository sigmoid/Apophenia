#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTex;

layout(location = 2) in vec2 inPositionOffset;
layout(location = 3) in vec2 inSize;
layout(location = 4) in vec2 inUV_Min;
layout(location = 5) in vec2 inUV_Max;
layout(location = 6) in float inTexID;
layout(location = 7) in vec4 inColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec2 texCoords;
layout(location = 1) out float texID;
layout(location = 2) out vec2 minUV;
layout(location = 3) out vec2 maxUV;
layout(location = 4) out vec4 outColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition * vec3(inSize,1.0) + vec3(inPositionOffset, 0.0), 1.0);
    texCoords = inTex;
    minUV = inUV_Min;
    maxUV = inUV_Max;
    texID = inTexID;
    outColor = inColor;
}
