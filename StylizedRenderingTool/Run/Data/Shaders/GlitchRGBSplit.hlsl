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

	float Amount;
	float CenterFading;
	float AmountR;
	float AmountB;

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

	half time = RandomNoise(Amount);
//TotalTime * Speed;
	half splitAmount = RandomNoise(Amount);
//(1.f + sin(time)) * 0.5f;
	//splitAmount *= (1.f + sin(time * 2) * 0.5f);
	splitAmount = pow(splitAmount, 3.f);
	splitAmount *= 0.05f;
	float distance = length(uv - float2(0.5, 0.5));
	splitAmount *= Fade * Amount;
	splitAmount *= Fade * Amount;

	float splitAmountR = splitAmount * AmountR;
	float splitAmountB = splitAmount * AmountB;

	half3 colorR = sceneTexture.Sample(sceneSampler, float2(uv.x + splitAmountR, uv.y + splitAmountR)).rgb;
	half4 sceneColor = sceneTexture.Sample(sceneSampler, uv);
	half3 colorB = sceneTexture.Sample(sceneSampler, float2(uv.x - splitAmountB, uv.y - splitAmountB)).rgb;

	half3 splitColor = half3(colorR.r, sceneColor.g, colorB.b);
	half3 finalColor = lerp(sceneColor.rgb, splitColor, Fade);
	
	OUT.color.rgb = finalColor;
	OUT.color.a = 1.f;
	return OUT;
}