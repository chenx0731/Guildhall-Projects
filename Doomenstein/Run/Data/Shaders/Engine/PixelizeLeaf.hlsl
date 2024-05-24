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
	float Fade;
	float Speed;   
	float PixelScaleX;
	float PixelScaleY;
	float PixelSize;
	float PixelRatio;
	//float Padding0;
	//float Padding1;
};
cbuffer ShaderManagerConstants : register(b5)
{
	float2 CursorRatio;
	float2 InvViewSize;
	
	float TotalTime;
	int   ComparisonMode;
	float Padding2;
	float Padding3;
}

float2 TrianglePixelizeUV(float2 uv)
{
	float2 pixelScale = PixelSize * float2(PixelScaleX, PixelScaleY / PixelRatio);
	
	float2 coord = floor(uv * pixelScale) / pixelScale;
	
	uv -= coord;
	uv *= pixelScale;
	
	coord += float2(step(1.0 - uv.y, uv.x) / (pixelScale.x), 
					step(uv.x, uv.y) / pixelScale.y);

	return coord;
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
	float2 uv = TrianglePixelizeUV(input.uv);
	OUT.color = sceneTexture.Sample(sceneSampler, uv);
	return OUT;
}