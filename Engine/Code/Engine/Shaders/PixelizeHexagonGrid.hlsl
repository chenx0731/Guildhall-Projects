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
	float PixelSize;
	float GridWidth;

};
cbuffer ShaderManagerConstants : register(b5)
{
	float2 CursorRatio;
	float2 InvViewSize;
	
	float TotalTime;
	int   ComparisonMode;
	float Padding0;
	float Padding1;
}
float HexDist(float2 a, float2 b)
{
	float2 p = abs(b - a);
	float s = 0.5; // 1/2
	float c = 0.8660254; // sqrt(3)/2
	
	float diagDist = s * p.x + c * p.y;
	return max(diagDist, p.x) / c;
}

float2 NearestHex(float s, float2 st)
{
/*
	float h = 0.5 * s;
	float r = 0.8660254 * s;
	float b = s + 2.0 * h;
	float a = 2.0 * r;
	float m = h / r;

	float2 sect = st / float2(2.0 * r, h + s);
	float2 sectPxl = fmod(st, float2(2.0 * r, h + s));
	
	float aSection = fmod(float(sect.y), 2.0);

	float2 coord = floor(sect);
	if (aSection > 0.0)
	{	
		if (sectPxl.y < (h - sectPxl.x * m))
		{
			coord -= 1.0; // ?
		}
		else if (sectPxl.y < (-h + sectPxl.x * m))
		{
			coord.y -= 2.0;
		}
	}
	else
	{
		if(sectPxl.x > r)
		{
			if(sectPxl.y < (2.0 * h - sectPxl.x * m))
			{
				coord.y -= 1.0;
			}
		}
		else
		{
			if(sectPxl.y < (sectPxl.x * m))
			{
				coord.y -= 1.0;
			}
			else
			{
				coord.x -= 1.0;
			}
		}
	}

	float xoff = fmod(coord.y, 2.0) * r;
	return float2(coord.x * 2.0 * r - xoff, coord.y * (h + s)) + float2(r * 2.0, s);
*/
		float h = 0.5 * s;
		float r = 0.8660254 * s;
		float b = s + 2.0 * h;
		float a = 2.0 * r;
		float m = h / r;
		
		float2 sect = st / float2(2.0 * r, h + s);
		float2 sectPxl = fmod(st, float2(2.0 * r, h + s));
		
		float aSection = fmod(floor(sect.y), 2.0);
		
		float2 coord = floor(sect);
		if (aSection > 0.0)
		{
			if(sectPxl.y < (h - sectPxl.x * m))
			{
				coord -= 1.0;
			}
			else if(sectPxl.y < (-h + sectPxl.x * m))
			{
				coord.y -= 1.0;
			}
		}
		else
		{
			if(sectPxl.x > r)
			{
				if(sectPxl.y < (2.0 * h - sectPxl.x * m))
				{
					coord.y -= 1.0;
				}
			}
			else
			{
				if(sectPxl.y < (sectPxl.x * m))
				{
					coord.y -= 1.0;
				}
				else
				{
					coord.x -= 1.0;
				}
			}
		}
		
		float xoff = fmod(coord.y, 2.0) * r;
		return float2(coord.x * 2.0 * r - xoff, coord.y * (h + s)) + float2(r * 2.0, s);
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
	float4 originColor = sceneTexture.Sample(sceneSampler, uv); 
	
	float2 screenSize = 1.0 / InvViewSize;
//float2(1.0 / InvViewSize.x, 1.0 / InvViewSize.y);
	float pixelSize = PixelSize * screenSize.x * 0.2;
	float2 nearest = NearestHex(pixelSize, uv * screenSize);
	float4 finalColor = sceneTexture.Sample(sceneSampler, nearest * InvViewSize);
	float dist = HexDist(uv * screenSize, nearest);
	
	float interiorSize = pixelSize;
	float interior = 1.0 - smoothstep(interiorSize - 0.8, interiorSize, dist * GridWidth);
	OUT.color = finalColor * interior;
	OUT.color.a = 1.f;
	if (ComparisonMode == 2) {
		if (input.uv.x < CursorRatio.x)
		OUT.color = originColor;
	}
	return OUT;
}