
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
    float4 CameraUpDirt;
    /*
	float2 DepthTextureSize;
    float Padding2;
    float Padding3;
    float4 WaterColor;*/
};
cbuffer CausticConstants : register(b6)
{
	float FilterRadius;
	float2 BlurDir;
	float BlurScale;

	float BlurDepthFalloff;
	float3 LightWorldPos;

	int	ImageWidth;
	int	ImageHeight;
	float SigmaD;
	float SigmaR;

	float2 MinSimulationBounds;
	float2 MaxSimulationBounds;
};
#ifndef groupthreads
#define groupthreads 32
#endif

#define MINF asfloat(0xff800000)
#define MAXF 10000.f
//------------------------------------------------------------------------------------------------
//Texture2D depthTexture : register(t2);

Texture2D<float> input : register(t0);
Texture2D<float> input4F : register(t1);

Texture2D<float> depth : register(t2);
Texture2D<float3> normal : register(t3);

RWTexture2D<float2> outputR : register(u0);
RWTexture2D<float2> outputG : register(u1);
RWTexture2D<float2> outputB : register(u2);

float GaussR(float sigma, float dist)
{
	return exp(-(dist * dist) / (2.f * sigma * sigma));
}

float GaussD(float sigma, int x, int y)
{
	return exp(-((x * x + y * y) / (2.f * sigma * sigma)));
}

static const float minFilterWeight = 10.0f;

float3 uvToEye(float2 coord, float z)
{
    float2 pos = coord * 2.f - 1.f;
    float4 clipPos = float4(pos.x, -pos.y, z, 1.f);
    float4 viewPos = mul(InvProjectionMatrix, clipPos);
    return (viewPos / viewPos.w).xyz;
}

float2 GetIntersection(float refractiveIndex, float floor)
{
	
}

[numthreads(groupthreads, groupthreads, 1)]
void CalculateCausticIntersection(int3 dTid : SV_DispatchThreadID)
{
	
	//float2 uv = (float2)dTid.xy / (MaxSimulationBounds - MinSimulationBounds);
	//int2 index = int2(uv.x * ImageWidth, uv.y * ImageHeight);
	float pixelDepth = depth[dTid.xy];
//depth.Sample(uv);
	float3 worldNormal = normal[dTid.xy].xyz;
	outputR[dTid.xy] = float2(MAXF, MAXF);
	outputG[dTid.xy] = float2(MAXF, MAXF);
	outputB[dTid.xy] = float2(MAXF, MAXF);
	if (pixelDepth >= 1.f || pixelDepth <= 0.f)
	{
		return;
	}
	float2 uv = float2((float)dTid.x / (float)ImageWidth, (float)dTid.y / (float)ImageHeight);
	float3 viewPos = uvToEye(uv, pixelDepth);
	float3 worldPos = mul(InvViewMatrix, float4(viewPos, 1.f)).xyz;
	float3 lightDir = worldPos - LightWorldPos;
	lightDir = normalize(lightDir);
	// Snell's law n1sin(m1) = n2sin(m2)
	float cosm1 = dot(-lightDir, worldNormal);
	if (cosm1 <= 0.f)
	{
		return;
	}
	float sinm1;
	if (cosm1 >= 1.f)
		sinm1 = 0.f;
	else sinm1 = sqrt(1 - cosm1 * cosm1);
/*
	float sinm2R = sinm1 / 1.33141f; // n1 = 1, n2R = 1.33141
	float sinm2G = sinm1 / 1.33659f; // n1 = 1, n2G = 1.33659
	float sinm2B = sinm1 / 1.34055f; // n1 = 1, n2B = 1.34055
*/
	float sinm2R = sinm1 / 1.3f; // n1 = 1, n2R = 1.33141
	float sinm2G = sinm1 / 1.35f; // n1 = 1, n2G = 1.33659
	float sinm2B = sinm1 / 1.4f; // n1 = 1, n2B = 1.34055
	float cosm2R = sqrt(1 - sinm2R * sinm2R);
	float cosm2G = sqrt(1 - sinm2G * sinm2G);
	float cosm2B = sqrt(1 - sinm2B * sinm2B);
	float3 refractDirR = -normalize(-lightDir - worldNormal) * sinm2R - worldNormal * cosm2R;
	float3 refractDirG = -normalize(-lightDir - worldNormal) * sinm2G - worldNormal * cosm2G;
	float3 refractDirB = -normalize(-lightDir - worldNormal) * sinm2B - worldNormal * cosm2B;

	float planeZ = -1.f;
	
	float impactTimeR = (planeZ - worldPos.z) / refractDirR.z;
	float impactTimeG = (planeZ - worldPos.z) / refractDirG.z;
	float impactTimeB = (planeZ - worldPos.z) / refractDirB.z;
	if (impactTimeR > 0.f)
	{
		float planeX = refractDirR.x * impactTimeR + worldPos.x;
		float planeY = refractDirR.y * impactTimeR + worldPos.y;
		outputR[dTid.xy] = float2(planeX, planeY);
	}
	if (impactTimeG > 0.f)
	{
		float planeX = refractDirG.x * impactTimeG + worldPos.x;
		float planeY = refractDirG.y * impactTimeG + worldPos.y;
		outputG[dTid.xy] = float2(planeX, planeY);
	}
	if (impactTimeB > 0.f)
	{
		float planeX = refractDirB.x * impactTimeB + worldPos.x;
		float planeY = refractDirB.y * impactTimeB + worldPos.y;
		outputB[dTid.xy] = float2(planeX, planeY);
	}
	return;
}