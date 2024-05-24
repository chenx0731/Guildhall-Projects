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

cbuffer CameraConstants : register(b2)
{
	float OrthoMinX;
	float OrthoMinY;
	float OrthoMinZ;
	float OrthoMaxX;
	float OrthoMaxY;
	float OrthoMaxZ;
	float pad0;
	float pad1;
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
	v2p.color = input.color;
	v2p.uv = input.uv;
	float4 clipPosition;
	clipPosition.x = RangeMap(input.localPosition.x, OrthoMinX, OrthoMaxX, -1, 1);
	clipPosition.y = RangeMap(input.localPosition.y, OrthoMinY, OrthoMaxY, -1, 1);
	clipPosition.z = RangeMap(input.localPosition.z, OrthoMinZ, OrthoMaxZ, 0, 1);
	//clipPosition.x = RangeMap(input.localPosition.x, 0, 1600, -1, 1);
	//clipPosition.y = RangeMap(input.localPosition.y, 0, 800, -1, 1);
	//clipPosition.z = RangeMap(input.localPosition.z, 0, 1, 0, 1);
	clipPosition.w = 1;
	v2p.position = clipPosition;
	return v2p;
}

float4 PixelMain(v2p_t input) : SV_Target0
{
	return float4(input.color);
}

