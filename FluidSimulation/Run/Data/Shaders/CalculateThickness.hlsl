#include "Constants.hlsl"
//-----------------------------------------------------------------------------------------------

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
    float size : TEXCOORD1;
};


struct PSOutput
{
    float4 color : SV_TARGET;
    //float depth : SV_DEPTH;
};
/*
struct PSOutput
{
    float4 color : SV_TARGET;
    //float depth : SV_DEPTH;
};*/
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
    float3 EyeSpacePos;
    float SphereRadius;
    float4 CameraUpDir;
    float4 CameraLeftDir;

}
//------------------------------------------------------------------------------------------------
Texture2D diffuseTexture : register(t0);
StructuredBuffer<Particle> ParticlesRO : register(t1);
Texture2D depthTexture : register(t2);
StructuredBuffer<float> DensityRO : register(t4);
//------------------------------------------------------------------------------------------------
SamplerState diffuseSampler : register(s0);

static const float2 g_texcoords[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };
//------------------------------------------------------------------------------------------------
VSOutput VertexMain(uint ID : SV_VertexID)
{
    VSOutput OUT = (VSOutput)0;
     uint index = ID / 4;
    uint corner = ID % 4;
    
    float3 up = CameraUpDir.xyz;
    float3 right = -CameraLeftDir.xyz;
    float size = SphereRadius;

    float4 worldCenter = float4(ParticlesRO[index].Position.xyz, 1.f);
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
    
    OUT.position = clipPosition;
    OUT.texCoord = g_texcoords[corner];
    return OUT;
}

PSOutput PixelMain(VSOutput input)
{
    PSOutput OUT;
	
	// calculate eye-space sphere normal from texture coordinates
    /*
    float3 N;
    N.xy = input.texCoord * 2.0 - 1.0;
    float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0)
        discard;
    N.z = sqrt(1.0 - r2);
    */
    float3 N;
    N.yz = input.texCoord * 2.0 - 1.0;
    N.y = -N.y;
    float r2 = dot(N.yz, N.yz);
    if (r2 > 1.0)
        discard;
    N.x = -sqrt(1.0 - r2);
    N.x = -1.f;    

    //OUT.color.yzw = OUT.color.xxx;
    //OUT.color = float4(-N.x * 0.05f, 0.f , 0.f, -N.x * 0.1f);
    OUT.color = float4(-N.x * 0.05f, 0.f , 0.f,  0.1f);
    if (OUT.color.a < 0.01f)
        discard;
    return OUT;
}

