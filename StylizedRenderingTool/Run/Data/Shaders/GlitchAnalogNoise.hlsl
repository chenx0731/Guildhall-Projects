struct VSInput
{
    uint vidx : SV_VERTEXID;
};

static float3 FS_POS[3] =
{
    float3(-1.0f, -1.0f, 0.0f),
  
    float3(3.0f, -1.0f, 0.0f),

    float3(-1.0f, 3.0f, 0.0f),
};

static float2 FS_UVS[3] =
{
    float2(0.0f, 1.0f),
    float2(2.0f, 1.0f),
    float2(0.0f, -1.0f),
};

//------------------------------------------------------------------------------------------------
struct VSOutput
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
	float4x4 ModelMatrix;
	float4 ModelColor;
};
cbuffer RenderConstants : register(b4)
{
    float CursorXRatio;
	float TotalTime;
	float Fade;
	float Speed;   

	float LuminanceJitterThreshold;

};

float RandomNoise(float2 seed)
{
	return frac(sin(dot(seed * floor(30.f * cos(TotalTime * Speed)), float2(127.1, 311.7))) * 43758.5453123);
}

float RandomNoise(float seed)
{	
	return RandomNoise(float2(seed, 1.0));
}

//------------------------------------------------------------------------------------------------
Texture2D sceneTexture : register(t2);
Texture2D depthTexture : register(t3);
//------------------------------------------------------------------------------------------------
SamplerState sceneSampler : register(s0);

//------------------------------------------------------------------------------------------------
VSOutput VertexMain(VSInput input)
{
	VSOutput v2p;
    v2p.position = float4(FS_POS[input.vidx], 1.f);
	v2p.color = 1.f.xxxx;
	v2p.uv = FS_UVS[input.vidx];
	return v2p;
}

//------------------------------------------------------------------------------------------------
PSOutput PixelMain(VSOutput input)
{ 
	PSOutput OUT;	
	float2 uv = input.uv;
	half4 sceneColor = sceneTexture.Sample(sceneSampler, uv);
	half4 noiseColor = sceneColor;

	half luminance = dot(noiseColor.rgb, half3(0.22, 0.707, 0.071));
	//float randomness = RandomNoise(float2(TotalTime * Speed, TotalTime * Speed));
	float randomness = RandomNoise(sin(TotalTime * Speed));
	if (randomness > LuminanceJitterThreshold)
	{	
		noiseColor = float4(luminance, luminance, luminance, luminance);
	}

	float noiseX = RandomNoise(TotalTime * Speed + uv / float2(-213, 5.53));
	float noiseY = RandomNoise(TotalTime * Speed - uv / float2(213, -5.53));
	float noiseZ = RandomNoise(TotalTime * Speed + uv / float2(213, 5.53));

	noiseColor.rgb += 0.25f * float3(noiseX, noiseY, noiseZ) - 0.125;
	noiseColor = lerp(sceneColor, noiseColor, Fade);
	OUT.color = noiseColor;
	return OUT;
}