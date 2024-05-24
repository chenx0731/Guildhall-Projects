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
    float2 InvViewSize;
	float2 BlurRadius;
	float CursorXRatio;
	float padding0;
	float padding1;
	float padding2;
};

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
	
	float4 color = sceneTexture.Sample(sceneSampler, input.uv);
	if (input.uv.x > CursorXRatio)
	{
		float maxRGB = max(color.r, max(color.g, color.b));	
		color.rgb = maxRGB.rrr;
		color += float4(0.5, 0.2, 0.0, 1);
	}
	float depth = depthTexture.Sample(sceneSampler, input.uv).r;
	for (float x = -2.f; x <= 2.f; x += 1.f)
	{
		for (float y = -2.f; y <= 2.f; y += 1.f)
		{
			float2 offset = input.uv + float2(x, y) * InvViewSize;
			float depthOfNeighbor = depthTexture.Sample(sceneSampler, offset).r;
			float diff = depth - depthOfNeighbor;
			if (abs(diff) > 0.005)
			{
				if (input.uv.x > CursorXRatio)
					color = float4(1, 0, 1, 1);
			}
		}
	}

	OUT.color = color;

	return OUT;
}