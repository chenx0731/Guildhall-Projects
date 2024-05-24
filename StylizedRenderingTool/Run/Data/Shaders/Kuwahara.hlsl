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
float g_sobelX[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
float g_sobelY[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
cbuffer RenderConstants : register(b4)
{
	float CursorXRatio;
	float TotalTime;
	float Fade;
	float Speed;

    float2 InvViewSize;
	float2 BlurRadius;

};


//float g_sobelX[9] = {1, 0, 1, 0, 0, 2, 2, 1, 2};
//float g_sobelY[9] = {0, 1, 1, 2, 0, 0, 1, 2, 2};

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
	float4 mean[4] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};

	float4 sigma[4] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};

	float4 originalColor = sceneTexture.Sample(sceneSampler, input.uv);
	
	float2 offsets[4] = {{-BlurRadius.x, -BlurRadius.y}, {-BlurRadius.x, 0}, {0, -BlurRadius.y}, {0, 0}};
	int index = 0;
	float2 texelSize = InvViewSize;
	float2 pos;
	float4 col;

	float gradientX = 0;
	float gradientY = 0;
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			
			if(index == 4)
			{
				index++;
				continue;
			}

			float2 offset = float2(x, y) * texelSize;
			float3 pxCol = sceneTexture.Sample(sceneSampler, input.uv + offset).rgb;
			//float pxLum = dot(pxCol, float3(0.2126, 0.7152, 0.0722));
			float pxLum = dot(pxCol, float3(0.7152, 0.0722, 0.2126));

			gradientX += pxLum * g_sobelX[index];
			gradientY += pxLum * g_sobelY[index];

			index++;

		}
	}

	float angle = 0;

	if(abs(gradientX) > 0.0001)
	{
		angle = atan2(gradientY, gradientX);
	}

	float s = sin(angle);
	float c = cos(angle);

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j <= BlurRadius.x; j++)
		{
			[unroll(20)]
			for(int k = 0; k <= BlurRadius.y; k++)
			{
				pos = float2(j, k) + offsets[i];
				float2 offs = pos * texelSize;

				offs = float2(offs.x * c - offs.y * s, offs.x * s + offs.y * c);

				float2 uvpos = input.uv + offs;
				col = sceneTexture.Sample(sceneSampler, uvpos);

				mean[i] += col;

				sigma[i] += col * col;

			}

		}
	}

	float n = (BlurRadius.x + 1) * (BlurRadius.y + 1);
	float sigma_f;
	float min = 10;

	for(int neighborIndex = 0; neighborIndex < 4; neighborIndex++)
	{
		mean[neighborIndex] /= n;
		sigma[neighborIndex] = abs(sigma[neighborIndex] / n - mean[neighborIndex] * mean[neighborIndex]);
		sigma_f = sigma[neighborIndex].r + sigma[neighborIndex].g + sigma[neighborIndex].b;

		if(sigma_f < min)
			{
				min = sigma_f;
				col = mean[neighborIndex];
			}
	}
	
	float depth = depthTexture.Sample(sceneSampler, input.uv).r;
	[unroll]
	for (float m = -1.f; m <= 1.f; m += 1.f)
	{
		for (float n = -1.f; n <= 1.f; n += 1.f)
		{
			float2 offset = input.uv + float2(m, n) * InvViewSize;
			float depthOfNeighbor = depthTexture.Sample(sceneSampler, offset).r;
			float diff = depth - depthOfNeighbor;
			if (abs(diff) > 0.005)
			{
				if (input.uv.x > CursorXRatio)
					col = float4(1, 1, 0, 1);
			}
		}
	}

	OUT.color = col;

	if (input.uv.x < CursorXRatio)
		OUT.color = originalColor;
	return OUT;
}