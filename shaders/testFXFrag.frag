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
    float screenWidth;
    float screenHeight;
} warp;

#define FXAA_SPAN_MAX	8.0
#define FXAA_REDUCE_MUL 1.0/8.0
#define FXAA_REDUCE_MIN 1.0/128.0

//========================================================================================
float Hash( float n )
{
    return fract(sin(n)*43758.5453123);
}

//========================================================================================
float Noise( in vec2 x )
{
	
    vec2 p = floor(x);
    vec2 f = fract(x);
	
   	f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0;
    float res = mix(mix( Hash(n+  0.0), Hash(n+  1.0),f.x),
                    mix( Hash(n+ 57.0), Hash(n+ 58.0),f.x),f.y);
    return res;
}

//========================================================================================
vec3 hsv(in float h) 
{
	if (h > .45 && h < .55) return vec3(1.0);
	vec3 rgb = clamp((abs(fract(h + vec3(3, 2, 1) / 3.0) * 6.0 - 3.0) - 1.0), 0.0 , 1.0);
	return sqrt(rgb);
}

//========================================================================================
vec3 FxaaPixelShader(vec2 uv, sampler2D texSampler, vec2 iResolution) 
{
	vec4 fragColor;

	vec2 add = vec2(1.0) / iResolution.xy;
			
	vec3 rgbNW = texture(texSampler, uv+vec2(-add.x, -add.y)).xyz;
	vec3 rgbNE = texture(texSampler, uv+vec2( add.x, -add.y)).xyz;
	vec3 rgbSW = texture(texSampler, uv+vec2(-add.x,  add.y)).xyz;
	vec3 rgbSE = texture(texSampler, uv+vec2( add.x,  add.y)).xyz;
	vec3 rgbM  = texture(texSampler, uv).xyz;
	
	vec3 luma	 = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	
	
	float dirReduce = max(
		(lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	  
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	

	dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
		  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
		  dir * rcpDirMin)) * add;

		
	vec3 rgbA = (1.0/2.0) * (texture(texSampler, uv + dir * (1.0/3.0 - 0.5)).xyz +
							 texture(texSampler, uv + dir * (2.0/2.0 - 0.5)).xyz);
	
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) *
		(texture(texSampler, uv.xy + dir * (0.0/3.0 - 0.5)).xyz +
		 texture(texSampler, uv.xy + dir * (3.0/3.0 - 0.5)).xyz);
	
	float lumaB = dot(rgbB, luma);
	if((lumaB < lumaMin) || (lumaB > lumaMax))
	{
		return rgbA;
	}else
	{
		return rgbB;
	}
}

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
            // debugR = noiseValue; //playerDist / 0.4; 
        }

         if(warp.screenHeight > 0)
         {
             vec3 col = FxaaPixelShader(newCoords, texSampler, vec2(warp.screenWidth,warp.screenHeight));
             outColor = vec4(col, 1.);
         }
         else
         {
            outColor = texture(texSampler, newCoords);
            outColor.a = 1.0;
         }
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
