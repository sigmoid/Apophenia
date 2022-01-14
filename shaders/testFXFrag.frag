#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2D noiseMap;

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform UniformBufferObject {
    float warpFactor;
    float xPadding;
    float yPadding;
    float playerX;
    float playerY;
    float displayBlur;
} warp;

void main() {
    vec2 newCoords = texCoords;
    float noiseValue;

    float debugR = 0.0;

    if(newCoords.x < warp.xPadding || newCoords.x > 1.0 - warp.xPadding || newCoords.y < warp.yPadding || newCoords.y > 1.0 - warp.yPadding)
    {
        outColor = vec4(0,0,0,0);
    }
    else
    {    
        if (warp.warpFactor != 1.0)
        {
            newCoords = vec2(pow(texCoords.x, warp.warpFactor), texCoords.y);
        }

        if(warp.displayBlur > 0)
        {
            noiseValue = texture(noiseMap, texCoords).r;

            float playerDist = distance(vec2(warp.playerX + warp.xPadding, warp.playerY + warp.yPadding), texCoords);

            if(playerDist < 0.4)
            {
                float damp = playerDist / 0.4;
                damp = pow(damp, 10);
                noiseValue *= damp;
            }

            newCoords.x += (noiseValue - 0.5) / 20;
            // debugR = playerDist / 0.4; 
        }

        outColor = texture(texSampler, newCoords);
    }

    if(warp.displayBlur < -100)
    {
        outColor = texture(noiseMap, texCoords);
    }

    // if(debugR > 0.001)
    // {
    //     outColor.r = debugR;
    // }
}
