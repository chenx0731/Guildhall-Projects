#include "Constants.hlsl"
//-----------------------------------------------------------------------------------------------

struct VSInput
{
    //uint vidx : SV_VERTEXID;
    uint instanceID : SV_InstanceID;
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
    float2 texCoord : TEXCOORD0;
};


struct PSOutput
{
    float4 color : SV_TARGET0;
};

cbuffer LightConstants : register(b1)
{
    float3 LightDirt;
    float  Shininess;

    float3 LightPos;
    float  FresPower;

    float  FresScale;
    float  FresBias;
    float  Padding0;
    float  Padding1;

    float4x4 InvSkyboxModelMatrix;
}

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
    float4x4 InvProjectionMatrix;
    float4x4 InvViewMatrix;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
	float4x4 ModelMatrix;
	float4 ModelColor;
};

cbuffer RenderConstants : register(b4)
{
    float3 EyeSpacePos;
    float SphereRadius;
    float4 CameraUpDir;
    float4 CameraLeftDir;
    float2 DepthTextureSize;
    float Padding2;
    float Padding3;
    float4 WaterColor;
}

cbuffer BlurConstants : register(b6)
{
	float FilterRadius;
	float2 BlurDir;
	float BlurScale;

	float BlurDepthFalloff;
	float3 Padding;

	uint    ImageWidth;
	uint	ImageHeight;
	float SigmaD;
	float SigmaR;
};

Texture2D<float2> intersectionsR : register(t0);
Texture2D<float2> intersectionsG : register(t1);
Texture2D<float2> intersectionsB : register(t2);
static const float2 g_texcoords[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };
#define MAXF 9999.f
//------------------------------------------------------------------------------------------------
VSOutput VertexMain(uint ID : SV_VertexID)
{
    VSOutput OUT = (VSOutput)0;
    OUT.color = float4(1.f, 0, 0, 1);
    int channel = 0;
    if (ID >= ImageWidth * ImageHeight * 4)
    {
        OUT.color = float4(0, 1.f, 0, 1);
        ID -= ImageWidth * ImageHeight * 4;
        channel = 1;
        if (ID >= ImageWidth * ImageHeight * 4)
        {
            OUT.color = float4(0, 0, 1.f, 1);
            channel = 2;
            ID -= ImageWidth * ImageHeight * 4;
        }
    }
    uint y = (ID / 4) / ImageWidth;
    uint x = (ID / 4) - y * ImageWidth;
    uint2 indexes = uint2(x,y);
    float2 intersection = intersectionsR[indexes].xy;
    if (channel == 1)
    intersection = intersectionsG[indexes].xy;
    else if (channel == 2)
    intersection = intersectionsB[indexes].xy;
    //if (intersection.x > MAXF)
        //discard;
    float4 worldCenter = float4(intersection, -0.99f, 1.f);

    float size = 0.1f ;
    
    
    uint index = ID / 4;
    uint corner = ID % 4;
    
    float3 up = float3(1.f, 0.f, 0.f);
    float3 right = float3(0.f, -1.f, 0.f);

    //float4 worldCenter = float4(ParticlesRO[index].Position.xyz, 1.f);
    float4 worldPosition = worldCenter;
    if (corner == 0)
    {
        worldPosition.xyz = worldCenter.xyz - right * size - up * size;
    }
    else if (corner == 1)
    {
        worldPosition.xyz = worldCenter.xyz + right * size - up * size;
    }
    else if (corner == 2)
    {
        worldPosition.xyz = worldCenter.xyz + right * size + up * size;
    }
    else if (corner == 3)
    {
        worldPosition.xyz = worldCenter.xyz - right * size + up * size;
    }

    float4 viewPosition = mul(ViewMatrix, worldPosition);
    float4 clipPosition = mul(ProjectionMatrix, viewPosition);
    
    //OUT.viewPos = viewPosition;
    OUT.position = clipPosition;
    OUT.texCoord = g_texcoords[corner];
    return OUT;
}



PSOutput PixelMain(VSOutput input)
{
    PSOutput OUT;
	
	// calculate eye-space sphere normal from texture coordinates
    
    float3 N;
    N.yz = input.texCoord * 2.0 - 1.0;
    N.y = -N.y;
    float r2 = dot(N.yz, N.yz);
    if (r2 > 1.0)
        discard;
    N.x = -sqrt(1.0 - r2);
    
    OUT.color = input.color;
    OUT.color.a = 0.005f * (-N.x * -N.x * -N.x);
//float4(1.f, 1.f , 1.f, 0.015f * -N.x);
    //if (OUT.color.a < 0.01f)
        //discard;
    return OUT;

}
