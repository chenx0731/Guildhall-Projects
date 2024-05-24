struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 localTangent : TANGENT;
	float3 localBinormal : BINORMAL;
	float3 localNormal : NORMAL;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float4 normal : NORMAL;
};

cbuffer LightingConstants : register(b1)
{
	float3 SunDirection;
	float SunIntensity;
	float4 AmbientIntensity;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
};

cbuffer ModelConstants : register(b3)
{
	float4x4 ModelMatrix;
	float4 ModelColor;
};

//------------------------------------------------------------------------------------------------
Texture2D diffuseTexture : register(t0);

//------------------------------------------------------------------------------------------------
SamplerState diffuseSampler : register(s0);


v2p_t VertexMain(vs_input_t input)
{
	float4 localPosition = float4(input.localPosition, 1);
	float4 worldPosition = mul(ModelMatrix, localPosition);
	float4 viewPosition = mul(ViewMatrix, worldPosition);
	float4 clipPosition = mul(ProjectionMatrix, viewPosition);

	float4 localNormal = float4(input.localNormal, 0);
	float4 worldNormal = mul(ModelMatrix, localNormal);


	v2p_t v2p;
	v2p.position = clipPosition;
	v2p.color = input.color;
	v2p.uv = input.uv;
	v2p.normal = worldNormal;
	return v2p;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(v2p_t input) : SV_Target0
{
	float4 inputNormalized = normalize(input.normal);
	float dotResult = dot(-SunDirection, inputNormalized);
	float saturatedResult = saturate(dotResult);
	float mulResult = mul(saturatedResult, SunIntensity);
	float totalLight = mulResult;
	float4 lightColor;
	lightColor.r = totalLight + AmbientIntensity.r;
	lightColor.g = totalLight + AmbientIntensity.g;
	lightColor.b = totalLight + AmbientIntensity.b;
	lightColor.a = totalLight + AmbientIntensity.a;

	float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
	float4 vertexColor = input.color;
	float4 modelColor = ModelColor;

	if (textureColor.a < 0.1f) discard;
	float4 color = textureColor * vertexColor * lightColor;
	clip(color.a - 0.99f);
	return float4(color);
}
