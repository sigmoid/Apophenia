#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) uniform UserDataObject {
    vec4[8] boxes;
} userData;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

// I don't understand this at all, but it produces continous 2d noise that looks pretty good!
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 newCoords = texCoords;
    bool inBox = false;

    for(int i = 0; i < 8; i++)
    {
        if(newCoords.x <= userData.boxes[i].z && newCoords.x >= userData.boxes[i].x && newCoords.y <= userData.boxes[i].w && newCoords.y >= userData.boxes[i].y)
        {
            inBox = true;

            float noiseValue = rand(vec2(texCoords.x / 500000, texCoords.y / 100000));

            newCoords.x += (noiseValue - 0.5) / 20;

            break;
        }
    }

    outColor = texture(texSampler, newCoords);
    
    if(inBox)
    {
        // outColor.r += 0.1f;

    }

}
