#pragma once
const char* g_defaultShader = R"(
struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

SamplerState diffuseSampler : register(s0);

Texture2D diffuseTexture : register(t0);

cbuffer ModelConstants : register(b3)
{
	float4x4 ModelMatrix;
	float4 ModelColor;
};

cbuffer CameraConstants : register(b2)
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
};

float Interpolate(float start, float end, float fractionTowardEnd)
{
	return start + (end - start) * fractionTowardEnd;
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	return Interpolate(outStart, outEnd, GetFractionWithinRange(inValue, inStart, inEnd));
}

v2p_t VertexMain(vs_input_t input)
{
	v2p_t v2p;
	//v2p.position = float4(input.localPosition, 1);
	//v2p.color = input.color;
	v2p.color = input.color;
	v2p.uv = input.uv;
	float4 worldPosition = mul(ModelMatrix, float4(input.localPosition, 1));
	float4 viewPosition = mul(ViewMatrix, worldPosition);
	float4 clipPosition = mul(ProjectionMatrix, viewPosition);
	v2p.position = clipPosition;
	return v2p;
}

float4 PixelMain(v2p_t input) : SV_Target0
{
	float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
	float4 vertexColor = input.color;
	float4 modelColor = ModelColor;

	if(textureColor.a < 0.1f) discard;
	float4 color = textureColor * vertexColor * modelColor;
	return float4(color);
}

)";