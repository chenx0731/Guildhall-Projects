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

	float Seed;
	float Offset;
	float BlockLayer1U;
	float BlockLayer1V;

	float BlockLayer2U;
	float BlockLayer2V;
	float BlockLayer1Intensity;
	float BlockLayer2Intensity;

	float BlockSplitIntensity;

};

float RandomNoise(float2 seed)
{
	return frac(sin(dot(seed * floor(TotalTime * Speed), float2(127.1, 311.7))) * 43758.5453123);
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
	
	float2 blockLayer1 = floor(uv * float2(BlockLayer1U, BlockLayer1V));
	float2 blockLayer2 = floor(uv * float2(BlockLayer2U, BlockLayer2V));

	float lineNoise1 = pow(RandomNoise(blockLayer1), BlockLayer1Intensity);
	float lineNoise2 = pow(RandomNoise(blockLayer2), BlockLayer2Intensity);
	float RGBSplitNoise = pow(RandomNoise(Seed), 7.1) * BlockSplitIntensity;
	float lineNoise = lineNoise1 * lineNoise2 * Offset - RGBSplitNoise;

	float4 colorR = sceneTexture.Sample(sceneSampler, uv);
	float4 colorG = sceneTexture.Sample(sceneSampler, uv + float2(lineNoise * 0.05 * RandomNoise(7), 0));
	float4 colorB = sceneTexture.Sample(sceneSampler, uv - float2(lineNoise * 0.05 * RandomNoise(23), 0));

	float4 result = float4(float3(colorR.x, colorG.y, colorB.z), colorR.a + colorG.a + colorB.a);
	result = lerp(colorR, result, Fade);
	OUT.color = result;
	return OUT;
}