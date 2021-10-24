#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform ModelMats
{
    mat4 model;
} obj;

layout(location = 0) out vec4 outColor;

void main() {
    gl_Position = ubo.proj * ubo.view * obj.model * vec4(inPosition, 0.0, 1.0);
    outColor = inColor;
}
