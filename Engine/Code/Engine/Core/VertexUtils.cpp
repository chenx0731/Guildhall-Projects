#define UNUSED(x) (void)(x);
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/ConvexPoly2D.hpp"

void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float scaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	for (int vertIndex = 0; vertIndex < numVerts; vertIndex++)
	{
		Vec3& pos = verts[vertIndex].m_position;
		TransformPositionXY3D(pos, scaleXY, rotationDegreesAboutZ, translationXY);
	}
}


void TransformVertexArrayXYZ3D(int numVerts, Vertex_PCU* verts, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation)
{
	for (int vertIndex = 0; vertIndex < numVerts; vertIndex++)
	{
		Vec3& pos = verts[vertIndex].m_position;
		TransformPositionXYZ3D(pos, iBasis, jBasis, kBasis, translation);
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform)
{
	for (int vertIndex = 0; vertIndex < verts.size(); vertIndex++) {
		verts[vertIndex].m_position = transform.TransformPosition3D(verts[vertIndex].m_position);
	}
}

void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, const Mat44& transform)
{
	for (int vertIndex = 0; vertIndex < verts.size(); vertIndex++) {
		verts[vertIndex].m_position = transform.TransformPosition3D(verts[vertIndex].m_position);
		verts[vertIndex].m_normal = transform.TransformVectorQuantity3D(verts[vertIndex].m_normal);
	}
}

void CalculateTangentSpaceBasisVectors(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	for (int i = 0; i < (int)indexes.size(); i += 3) {
		unsigned int i0 = indexes[i];
		unsigned int i1 = indexes[i + 1];
		unsigned int i2 = indexes[i + 2];
		Vertex_PCUTBN v0 = verts[i0];
		Vertex_PCUTBN v1 = verts[i1];
		Vertex_PCUTBN v2 = verts[i2];
		//Vec3 e0 = v1.m_position - v0.m_position;
		//Vec3 e1 = v2.m_position - v0.m_position;
		Vec2 w0 = v0.m_uvTexCoords;
		Vec2 w1 = v1.m_uvTexCoords;
		Vec2 w2 = v2.m_uvTexCoords;
		//float x0 = w1.x - w0.x;
		//float x1 = w2.x - w0.x;
		//float y0 = w1.y - w0.y;
		//float y1 = w2.y - w0.y;
		//float r = 1.f / (x0 * y1 - x1 * y0);
		//Vec3 tangent = (e0 * y1 - e1 * y0) * r;
		//Vec3 binormal = (e1 * x0 - e0 * x1) * r;
		//Vec3 normal = CrossProduct3D(e0, e1);
		
		Vec3 e1 = v1.m_position - v0.m_position;
		Vec3 e2 = v2.m_position - v0.m_position;
		float x1 = w1.x - w0.x, x2 = w2.x - w0.x;
		float y1 = w1.y - w0.y, y2 = w2.y - w0.y;
		float r = 1.0f / (x1 * y2 - x2 * y1);
		Vec3 tangent = (e1 * y2 - e2 * y1) * r;
		Vec3 binormal = (e2 * x1 - e1 * x2) * r;

		verts[i0].m_tangent += tangent;
		verts[i0].m_binormal += binormal;
		//verts[i0].m_normal += normal;

		verts[i1].m_tangent += tangent;
		verts[i1].m_binormal += binormal;
		//verts[i1].m_normal += normal;

		verts[i2].m_tangent += tangent;
		verts[i2].m_binormal += binormal;
		//verts[i2].m_normal += normal;
	}

	for (int vertIndex = 0; vertIndex < (int)verts.size(); vertIndex++) {
		Vec3 n = verts[vertIndex].m_normal;
		Vec3 b = verts[vertIndex].m_binormal;
		Vec3 t = verts[vertIndex].m_tangent;

		Mat44 mat;
		

		Vec3 correctT = (t - DotProduct3D(t, n) * n).GetNormalized();
		Vec3 correctB = (b - DotProduct3D(b, n) * n - DotProduct3D(b, correctT) * correctT).GetNormalized();

		//verts[vertIndex].m_normal = n;
		verts[vertIndex].m_binormal = correctB;
		verts[vertIndex].m_tangent = correctT;
	}

}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec3 BL = Vec3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	Vec3 BR = Vec3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);
	Vec3 TR = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);
	Vec3 TL = Vec3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
}

void AddVertsForAABB2DWithPaddings(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, float top, float bottom, float left, float right)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec3 BL = Vec3(bounds.m_mins.x + left, bounds.m_mins.y + bottom, 0.f);
	Vec3 BR = Vec3(bounds.m_maxs.x - right, bounds.m_mins.y + bottom, 0.f);
	Vec3 TR = Vec3(bounds.m_maxs.x - right, bounds.m_maxs.y - top, 0.f);
	Vec3 TL = Vec3(bounds.m_mins.x + left, bounds.m_maxs.y - top, 0.f);

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec3 BL = Vec3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	Vec3 BR = Vec3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);
	Vec3 TR = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);
	Vec3 TL = Vec3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	verts.push_back(Vertex_PCU(BL, color, uvAtMins));
	verts.push_back(Vertex_PCU(BR, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
	verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));

	verts.push_back(Vertex_PCU(BL, color, uvAtMins));
	verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));
	verts.push_back(Vertex_PCU(TL, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
}

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	constexpr int NUM_TRIS = 12;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec2 uvAtMins = UVs.m_mins;
	Vec2 uvAtMaxs = UVs.m_maxs;

	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;

	// +x
	AddVertsForQuad3D(verts, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), color, UVs);
	// -x
	AddVertsForQuad3D(verts, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), color, UVs);
	// +y
	AddVertsForQuad3D(verts, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), color, UVs);
	// -y
	AddVertsForQuad3D(verts, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), color, UVs);
	// +z
	AddVertsForQuad3D(verts, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), color, UVs);
	// -z
	AddVertsForQuad3D(verts, Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), color, UVs);
}

void AddVertsForAABBWireframe3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, float lineThickness, Rgba8 const& tint)
{
	constexpr int NUM_TRIS = 12;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	float minX = bounds.m_mins.x;
	float minY = bounds.m_mins.y;
	float minZ = bounds.m_mins.z;
	float maxX = bounds.m_maxs.x;
	float maxY = bounds.m_maxs.y;
	float maxZ = bounds.m_maxs.z;

	AddVertsForLineSegment3D(verts, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), lineThickness, tint);

	AddVertsForLineSegment3D(verts, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), lineThickness, tint);
	
	AddVertsForLineSegment3D(verts, Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), lineThickness, tint);
	
	AddVertsForLineSegment3D(verts, Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), lineThickness, tint);
	AddVertsForLineSegment3D(verts, Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), lineThickness, tint);
}

void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, const Vec3& center, float radius, const Rgba8& color, const AABB2& UVs, int numLatitudePieces)
{
	int NUM_TRIS = numLatitudePieces * numLatitudePieces * 4;
	int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);
	float longitutdeDegree = 360.f / static_cast<float>(numLatitudePieces * 2);
	float latitudeDegree = 180.f / static_cast<float>(numLatitudePieces);
	float uPerPieces = UVs.GetDimensions().x / static_cast<float>(numLatitudePieces * 2);
	float vPerPieces = UVs.GetDimensions().y / static_cast<float>(numLatitudePieces); 


	for (int loIndex = 0; loIndex < numLatitudePieces * 2; loIndex++) {
		for (int laIndex = 0; laIndex < numLatitudePieces; laIndex++) {

			Vec3 BL = Vec3::MakeFromPolarDegrees(-90.f + (float)laIndex * latitudeDegree, (float)loIndex * longitutdeDegree, radius) + center;
			Vec3 BR = Vec3::MakeFromPolarDegrees(-90.f + (float)laIndex * latitudeDegree, (float)(loIndex + 1) * longitutdeDegree, radius) + center;
			Vec3 TL = Vec3::MakeFromPolarDegrees(-90.f + (float)(laIndex + 1) * latitudeDegree, (float)loIndex * longitutdeDegree, radius) + center;
			Vec3 TR = Vec3::MakeFromPolarDegrees(-90.f + (float)(laIndex + 1) * latitudeDegree, (float)(loIndex + 1) * longitutdeDegree, radius) + center;
			
			Vec2 uvAtMins = Vec2(uPerPieces * (float)(loIndex), vPerPieces * (float)(laIndex));
			Vec2 uvAtMaxs = Vec2(uPerPieces * (float)(loIndex + 1), vPerPieces * (float)(laIndex + 1));

			verts.push_back(Vertex_PCU(BL, color, uvAtMins));
			verts.push_back(Vertex_PCU(BR, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
			verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));

			verts.push_back(Vertex_PCU(BL, color, uvAtMins));
			verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));
			verts.push_back(Vertex_PCU(TL, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
		}
	}

}

void AddVertsForCylinderZ3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices, Rgba8 const& color, AABB2 const& UV)
{
	int numofSlices = RoundDownToInt(numSlices);
	if ((float)numofSlices != numSlices)
		numofSlices++;
	int NUM_VERTS = 3 * numofSlices * 2 * 3;
	verts.reserve(verts.size() + NUM_VERTS);

	float degreePerSlice = 360.f / numSlices;
	float uPerSlice = (UV.m_maxs.x - UV.m_mins.x) / numSlices;
	float startDegree = 0.f;
	float startU = UV.m_mins.x;

	for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
		startDegree = (float)sliceIndex * degreePerSlice;
		startU = (float)sliceIndex * uPerSlice;
		float endDegree = startDegree + degreePerSlice;
		float endU = startU + uPerSlice;
		if (endDegree > 360.f)
			endDegree = 360.f;
		if (endU > UV.m_maxs.x)
			endU = UV.m_maxs.x;
		Vec3 topCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_max);
		Vec3 bottomCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_min);
		Vec2 xyL = Vec2::MakeFromPolarDegrees(startDegree, radius) + centerXY;
		Vec2 xyR = Vec2::MakeFromPolarDegrees(endDegree, radius) + centerXY;
		Vec3 BL = Vec3(xyL.x, xyL.y, minMaxZ.m_min);
		Vec3 TL = Vec3(xyL.x, xyL.y, minMaxZ.m_max);
		Vec3 BR = Vec3(xyR.x, xyR.y, minMaxZ.m_min);
		Vec3 TR = Vec3(xyR.x, xyR.y, minMaxZ.m_max);
		AABB2 uv = AABB2(startU, UV.m_mins.y, endU, UV.m_maxs.y);
		AddVertsForQuad3D(verts, BL, BR, TR, TL, color, uv);
		Vec2 centerUV = UV.GetCenter();
		Vec2 uvStart = Vec2::MakeFromPolarDegrees(startDegree, centerUV.y) + centerUV;
		Vec2 uvEnd = Vec2::MakeFromPolarDegrees(endDegree, centerUV.y) + centerUV;
		AddVertsForQuad3D(verts, TL, TR, topCenter, topCenter, color, uvStart, uvEnd , centerUV, centerUV);
		uvStart.y = UV.m_maxs.y - uvStart.y;
		uvEnd.y = UV.m_maxs.y - uvEnd.y;
		//AddVertsForQuad3D(verts, BR, BL, bottomCenter, bottomCenter, color, uvEnd, uvStart , Vec2(0.5f, 0.5f), Vec2(0.5f, 0.5f));
		AddVertsForQuad3D(verts, bottomCenter, bottomCenter, BR, BL, color, centerUV, centerUV, uvEnd, uvStart);
	}
}

void AddVertsForCylinderZWireframe3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices, float lineThickness, Rgba8 const& color)
{
	int numofSlices = RoundDownToInt(numSlices);
	if ((float)numofSlices != numSlices)
		numofSlices++;
	
	float degreePerSlice = 360.f / numSlices;
	float startDegree = 0.f;

	for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
		startDegree = (float)sliceIndex * degreePerSlice;
		float endDegree = startDegree + degreePerSlice;
		if (endDegree > 360.f)
			endDegree = 360.f;
		Vec3 topCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_max);
		Vec3 bottomCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_min);
		Vec2 xyL = Vec2::MakeFromPolarDegrees(startDegree, radius) + centerXY;
		Vec2 xyR = Vec2::MakeFromPolarDegrees(endDegree, radius) + centerXY;
		Vec3 BL = Vec3(xyL.x, xyL.y, minMaxZ.m_min);
		Vec3 TL = Vec3(xyL.x, xyL.y, minMaxZ.m_max);
		Vec3 BR = Vec3(xyR.x, xyR.y, minMaxZ.m_min);
		Vec3 TR = Vec3(xyR.x, xyR.y, minMaxZ.m_max);
		AddVertsForLineSegment3D(verts, BL, BR, lineThickness, color);
		AddVertsForLineSegment3D(verts, BL, TL, lineThickness, color);
		AddVertsForLineSegment3D(verts, BR, TR, lineThickness, color);
		AddVertsForLineSegment3D(verts, TL, TR, lineThickness, color);
	}
}

void AddVertsForUVSphereZ3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& color, AABB2 const& UVs)
{
	int numofSlices = RoundDownToInt(numSlices);
	int numofStacks = RoundDownToInt(numStacks);
	if ((float)numofSlices != numSlices)
		numofSlices++;
	if ((float)numofStacks != numStacks)
		numofStacks++;
	int NUM_VERTS = numofSlices * numofSlices * 2 * 3;
	verts.reserve(verts.size() + NUM_VERTS);

	float degreePerSlice = 360.f / numSlices;
	float degreePerStack = 180.f / numStacks;

	float uPerSlice = UVs.GetDimensions().x / numSlices;
	float vPerStack = UVs.GetDimensions().y / numStacks;

	for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
		for (int stackIndex = 0; stackIndex < numofStacks; stackIndex++) {

			float startSliceDegree =  GetClamped((float)sliceIndex * degreePerSlice, 0.f, 360.f);
			float endSliceDegree = GetClamped((float)(sliceIndex + 1) * degreePerSlice, 0.f, 360.f);
			
			float startStackDegree = GetClamped((float)stackIndex * degreePerStack - 90.f, -90.f, 90.f);
			float endStackDegree = GetClamped((float)(stackIndex + 1) * degreePerStack - 90.f, -90.f, 90.f);

			float startSliceU = GetClamped((float)sliceIndex * uPerSlice + UVs.m_mins.x, UVs.m_mins.x, UVs.m_maxs.x);
			float endSliceU = GetClamped((float)(sliceIndex + 1) * uPerSlice + UVs.m_mins.x, UVs.m_mins.x, UVs.m_maxs.x);

			float startStackV = GetClamped((float)stackIndex * vPerStack + UVs.m_mins.y, UVs.m_mins.y, UVs.m_maxs.y);
			float endStackV = GetClamped((float)(stackIndex + 1) * vPerStack + UVs.m_mins.y, UVs.m_mins.y, UVs.m_maxs.y);


			Vec3 BL = Vec3::MakeFromPolarDegrees(startStackDegree, startSliceDegree, radius) + center;
			Vec3 BR = Vec3::MakeFromPolarDegrees(startStackDegree, endSliceDegree, radius) + center;
			Vec3 TL = Vec3::MakeFromPolarDegrees(endStackDegree, startSliceDegree, radius) + center;
			Vec3 TR = Vec3::MakeFromPolarDegrees(endStackDegree, endSliceDegree, radius) + center;

			Vec2 uvAtMins = Vec2(startSliceU, startStackV);
			Vec2 uvAtMaxs = Vec2(endSliceU, endStackV);

			verts.push_back(Vertex_PCU(BL, color, uvAtMins));
			verts.push_back(Vertex_PCU(BR, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
			verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));

			verts.push_back(Vertex_PCU(BL, color, uvAtMins));
			verts.push_back(Vertex_PCU(TR, color, uvAtMaxs));
			verts.push_back(Vertex_PCU(TL, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
		}
	}
}

void AddVertsForUVSphereZ3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	int numofSlices = RoundDownToInt(numSlices);
	int numofStacks = RoundDownToInt(numStacks);
	if ((float)numofSlices != numSlices)
		numofSlices++;
	if ((float)numofStacks != numStacks)
		numofStacks++;
	int NUM_VERTS = numofSlices * numofSlices * 2 * 3;
	verts.reserve(verts.size() + NUM_VERTS);

	float degreePerSlice = 360.f / numSlices;
	float degreePerStack = 180.f / numStacks;

	float uPerSlice = UVs.GetDimensions().x / numSlices;
	float vPerStack = UVs.GetDimensions().y / numStacks;

	/*
	int numofVerts = (numofStacks + 1) * (numSlices + 1);
	int numofIndexes = (numofStacks) * numofSlices * 6;
	verts.reserve(verts.size() + numofVerts);
	indexes.reserve(indexes.size() + numofIndexes);
	
	for (int stackIndex = 0; stackIndex <= numofStacks; stackIndex++) {
		for (int sliceIndex = 0; sliceIndex <= numofSlices; sliceIndex++) {
			float startSliceDegree = GetClamped((float)sliceIndex * degreePerSlice, 0.f, 360.f);
			float startStackDegree = GetClamped((float)stackIndex * degreePerStack - 90.f, -90.f, 90.f);
			float startSliceU = GetClamped((float)sliceIndex * uPerSlice + UVs.m_mins.x, UVs.m_mins.x, UVs.m_maxs.x);
			float startStackV = GetClamped((float)stackIndex * vPerStack + UVs.m_mins.y, UVs.m_mins.y, UVs.m_maxs.y);
			Vec3 BL = Vec3::MakeFromPolarDegrees(startStackDegree, startSliceDegree, radius) + center;
			Vec2 uvAtMins = Vec2(startSliceU, startStackV);

			verts.push_back(Vertex_PCUTBN(BL, color, uvAtMins));
		}
	}
	
	for (int stackIndex = 0; stackIndex < numofStacks; stackIndex++) {
		for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
			int BL = stackIndex * (numofSlices + 1) + sliceIndex;
			int BR = stackIndex * (numofSlices + 1) + (sliceIndex + 1);
			int TL = (stackIndex + 1) * (numofSlices + 1) + sliceIndex;
			int TR = (stackIndex + 1) * (numofSlices + 1) + (sliceIndex + 1);

			indexes.push_back(BL);
			indexes.push_back(BR);
			indexes.push_back(TR);

			indexes.push_back(BL);
			indexes.push_back(TR);
			indexes.push_back(TL);
		}
	}*/

	int numofVerts = 2 + (numofStacks - 1) * numofSlices;
	int numofIndexes = 3 * ((numofStacks - 2) * numofSlices * 2 + 2 * numofSlices);
	verts.reserve(verts.size() + numofVerts);
	indexes.reserve(indexes.size() + numofIndexes);

	int base = (int)verts.size();

	Vertex_PCUTBN bottom;
	bottom.m_position = center - radius * Vec3(0.f, 0.f, 1.f);
	bottom.m_color = color;
	bottom.m_uvTexCoords = Vec2(0.f, 0.f);
	verts.push_back(bottom);

	for (int stackIndex = 0; stackIndex < numofStacks; stackIndex++) {
		if (stackIndex == 0)
			continue;
		for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
			float startSliceDegree = GetClamped((float)sliceIndex * degreePerSlice, 0.f, 360.f);
			float startStackDegree = GetClamped((float)stackIndex * degreePerStack - 90.f, -90.f, 90.f);
			float startSliceU = GetClamped((float)sliceIndex * uPerSlice + UVs.m_mins.x, UVs.m_mins.x, UVs.m_maxs.x);
			float startStackV = GetClamped((float)stackIndex * vPerStack + UVs.m_mins.y, UVs.m_mins.y, UVs.m_maxs.y);
			Vec3 BL = Vec3::MakeFromPolarDegrees(startStackDegree, startSliceDegree, radius) + center;
			Vec2 uvAtMins = Vec2(startSliceU, startStackV);

			verts.emplace_back(Vertex_PCUTBN(BL, color, uvAtMins));
		}
	}

	Vertex_PCUTBN top;
	top.m_position = center + radius * Vec3(0.f, 0.f, 1.f);
	top.m_color = color;
	top.m_uvTexCoords = Vec2(1.f, 1.f);
	verts.push_back(top);
	int full = (int)verts.size() - 1;
	for (int stackIndex = 0; stackIndex < numofStacks; stackIndex++) {
		for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
			if (stackIndex == 0) {
				indexes.push_back(base);
				if (sliceIndex != numofSlices - 1)
					indexes.push_back(base + sliceIndex + 2);
				else indexes.push_back(base + 1);
				indexes.push_back(base + sliceIndex + 1);
			}
			else if (stackIndex == numofStacks - 1) {
				indexes.push_back(full);
				//if (sliceIndex != numofSlices - 1)
					indexes.push_back(base + (stackIndex - 1) * numofSlices + sliceIndex + 1);
				//else indexes.push_back(base + (stackIndex - 1) * numofSlices + 1);
				if (sliceIndex == numofSlices - 1)
						indexes.push_back(base + (stackIndex - 1) * numofSlices + 1);
				else indexes.push_back(base + (stackIndex - 1) * numofSlices + sliceIndex + 2);
			}
			else {
				int BL = base + (stackIndex - 1) * numofSlices + sliceIndex + 1;
				int BR = base + (stackIndex - 1) * numofSlices + sliceIndex + 2;
				int TL = base + (stackIndex) * numofSlices + sliceIndex + 1;
				int TR = base + (stackIndex) * numofSlices + sliceIndex + 2;

				if (sliceIndex == numofSlices - 1)
				{
					BR = base + (stackIndex - 1) * numofSlices + 1;
					TR = base + (stackIndex)*numofSlices + 1;
				}
				Vec3 x = verts[BR].m_position - verts[BL].m_position;
				Vec3 y = verts[TR].m_position - verts[BR].m_position;

				Vec3 normal = CrossProduct3D(x, y).GetNormalized();
				verts[BL].m_normal = normal;
				verts[BR].m_normal = normal;
				verts[TL].m_normal = normal;
				verts[TR].m_normal = normal;

				indexes.push_back(BL);
				indexes.push_back(BR);
				indexes.push_back(TR);

				indexes.push_back(BL);
				indexes.push_back(TR);
				indexes.push_back(TL);
			}
		}
	}
}

void AddVertsForTBNDebug(std::vector<Vertex_PCU>& vertexes, const std::vector<Vertex_PCUTBN>& vertsTBN)
{
	int size = (int)vertsTBN.size() * 6;
	vertexes.reserve((int)vertexes.size() + size);
	for (int vertIndex = 0; vertIndex < (int)vertsTBN.size(); vertIndex++) {
		Vertex_PCU startN, endN;
		Vertex_PCU startT, endT;
		Vertex_PCU startB, endB;
		startN.m_position = vertsTBN[vertIndex].m_position;
		endN.m_position = vertsTBN[vertIndex].m_position + vertsTBN[vertIndex].m_normal * 0.25f;

		startT.m_position = vertsTBN[vertIndex].m_position;
		endT.m_position = vertsTBN[vertIndex].m_position + vertsTBN[vertIndex].m_tangent * 0.25f;

		startB.m_position = vertsTBN[vertIndex].m_position;
		endB.m_position = vertsTBN[vertIndex].m_position + vertsTBN[vertIndex].m_binormal * 0.25f;

		startN.m_color = Rgba8::BLUE;
		endN.m_color = Rgba8::BLUE;

		startT.m_color = Rgba8::RED;
		endT.m_color = Rgba8::RED;

		startB.m_color = Rgba8::GREEN;
		endB.m_color = Rgba8::GREEN;

		vertexes.push_back(startN);
		vertexes.push_back(endN);

		vertexes.push_back(startT);
		vertexes.push_back(endT);

		vertexes.push_back(startB);
		vertexes.push_back(endB);
	}
}

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const ConvexPoly2D& convexPoly, Rgba8 color /*= Rgba8::WHITE*/)
{
	int initial = (int)verts.size();
	std::vector<Vec2> points = convexPoly.GetAllPoints();
	for (int vertIndex = 0; vertIndex < (int)points.size(); vertIndex++) {
		Vertex_PCU vert;
		vert.m_position = points[vertIndex];
		vert.m_color = color;
		verts.push_back(vert);
	}

	for (int vertIndex = 1; vertIndex < (int)points.size() - 1; vertIndex++) {
		indexes.push_back(initial); 
		indexes.push_back(initial + vertIndex);
		indexes.push_back(initial + vertIndex + 1);
	}
}

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, const ConvexPoly2D& convexPoly, Rgba8 color /*= Rgba8::WHITE*/)
{
	std::vector<Vec2> points = convexPoly.GetAllPoints();
	for (int vertIndex = 1; vertIndex < (int)points.size() - 1; vertIndex++) {
		Vertex_PCU vert0, vert1, vert2;
		
		vert0.m_position = points[0];
		vert0.m_color = color;

		vert1.m_position = points[vertIndex];
		vert1.m_color = color;

		vert2.m_position = points[vertIndex + 1];
		vert2.m_color = color;

		verts.push_back(vert0);
		verts.push_back(vert1);
		verts.push_back(vert2);
	}
}

void AddVertsForConvexPoly2DFrame(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const ConvexPoly2D& convexPoly, float thickness, Rgba8 color /*= Rgba8::WHITE*/)
{
	// #SD4Convex: last edge is not included, and it's not the most optimized

	std::vector<Vec2> points = convexPoly.GetAllPoints();
	for (int pointIndex = 0; pointIndex < (int)points.size(); pointIndex++) {
		/*
		Vertex_PCU BL, BR;
		Vec2 start = points[pointIndex];
		Vec2 end;
		if (pointIndex == (int)points.size() - 1)
			end = points[0];
		else end = points[pointIndex + 1];
		Vec2 forward = end - start;
		Vec2 left = forward.GetRotated90Degrees();
		left.Normalize();
		
		BL.m_position = start + half * left;
		BR.m_position = start - half * left;

		BL.m_color = color;
		BR.m_color = color;

		verts.push_back(BL);
		verts.push_back(BR);

		if (pointIndex != (int)points.size() - 1) {
			indexes.push_back(initial + pointIndex * 2 );
			indexes.push_back(initial + pointIndex * 2 + 1);
			indexes.push_back(initial + pointIndex * 2 + 3);

			indexes.push_back(initial + pointIndex * 2);
			indexes.push_back(initial + pointIndex * 2 + 3);
			indexes.push_back(initial + pointIndex * 2 + 2);
		}
		else {
			indexes.push_back(initial + pointIndex * 2);
			indexes.push_back(initial + pointIndex * 2 + 1);
			indexes.push_back(initial + 1);

			indexes.push_back(initial + pointIndex * 2);
			indexes.push_back(initial + 1);
			indexes.push_back(initial);
		}*/

		Vec2 start = points[pointIndex];
		Vec2 end;
		if (pointIndex == (int)points.size() - 1)
			end = points[0];
		else end = points[pointIndex + 1];

		AddVertsForLineSegment2D(verts, indexes, start, end, thickness, color);
	}

}

void AddVertsForConvexPoly2DFrame(std::vector<Vertex_PCU>& verts, const ConvexPoly2D& convexPoly, float thickness, Rgba8 color /*= Rgba8::WHITE*/)
{
	// #SD4Convex
	float half = 0.5f * thickness;
	std::vector<Vec2> points = convexPoly.GetAllPoints();
	for (int pointIndex = 0; pointIndex < (int)points.size(); pointIndex++) {
		Vertex_PCU BL, BR, TL, TR;
		Vec2 start = points[pointIndex];
		Vec2 end;
		if (pointIndex == (int)points.size() - 1)
			end = points[0];
		else end = points[pointIndex + 1];
		Vec2 forward = end - start;
		Vec2 left = forward.GetRotated90Degrees();
		left.Normalize();

		BL.m_position = start + half * left;
		BR.m_position = start - half * left;
		TL.m_position = end + half * left;
		TR.m_position = end - half * left;

		BL.m_color = color;
		BR.m_color = color;
		TL.m_color = color;
		TR.m_color = color;

		verts.push_back(BL);
		verts.push_back(BR);
		verts.push_back(TR);

		verts.push_back(BL);
		verts.push_back(TR);
		verts.push_back(TL);
	}
}

void AddVertsForUVSphereZWireframe3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices, float numStacks, float lineThickness, Rgba8 const& color)
{
	int numofSlices = RoundDownToInt(numSlices);
	int numofStacks = RoundDownToInt(numStacks);
	if ((float)numofSlices != numSlices)
		numofSlices++;
	if ((float)numofStacks != numStacks)
		numofStacks++;
	int NUM_VERTS = numofSlices * numofSlices * 2 * 3;
	verts.reserve(verts.size() + NUM_VERTS);

	float degreePerSlice = 360.f / numSlices;
	float degreePerStack = 180.f / numStacks;

	for (int sliceIndex = 0; sliceIndex < numofSlices; sliceIndex++) {
		for (int stackIndex = 0; stackIndex < numofStacks; stackIndex++) {

			float startSliceDegree = GetClamped((float)sliceIndex * degreePerSlice, 0.f, 360.f);
			float endSliceDegree = GetClamped((float)(sliceIndex + 1) * degreePerSlice, 0.f, 360.f);

			float startStackDegree = GetClamped((float)stackIndex * degreePerStack - 90.f, -90.f, 90.f);
			float endStackDegree = GetClamped((float)(stackIndex + 1) * degreePerStack - 90.f, -90.f, 90.f);

			Vec3 BL = Vec3::MakeFromPolarDegrees(startStackDegree, startSliceDegree, radius) + center;
			Vec3 BR = Vec3::MakeFromPolarDegrees(startStackDegree, endSliceDegree, radius) + center;
			Vec3 TL = Vec3::MakeFromPolarDegrees(endStackDegree, startSliceDegree, radius) + center;
			Vec3 TR = Vec3::MakeFromPolarDegrees(endStackDegree, endSliceDegree, radius) + center;

			AddVertsForLineSegment3D(verts, BL, BR, lineThickness, color);
			AddVertsForLineSegment3D(verts, BL, TL, lineThickness, color);
			AddVertsForLineSegment3D(verts, TL, TR, lineThickness, color);
			AddVertsForLineSegment3D(verts, BR, TR, lineThickness, color);
		
		}
	}
}

AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts)
{
	AABB2 bounds;
	bounds.m_maxs.x = -999999999.f;
	bounds.m_maxs.y = -999999999.f;
	bounds.m_mins.x = 999999999.f;
	bounds.m_mins.y = 999999999.f;
	for (int vertIndex = 0; vertIndex < verts.size(); vertIndex++) {
		if (verts[vertIndex].m_position.x < bounds.m_mins.x) {
			bounds.m_mins.x = verts[vertIndex].m_position.x;
		}
		if (verts[vertIndex].m_position.y < bounds.m_mins.y) {
			bounds.m_mins.y = verts[vertIndex].m_position.y;
		}
		if (verts[vertIndex].m_position.x > bounds.m_maxs.x) {
			bounds.m_maxs.x = verts[vertIndex].m_position.x;
		}
		if (verts[vertIndex].m_position.y > bounds.m_maxs.y) {
			bounds.m_maxs.y = verts[vertIndex].m_position.y;
		}
	}
	return bounds;
}

void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	float numofSlice = (float)numSlices;
	Vec3 kBasis = end - start;
	//float length = kBasis.GetLength();
	//AABB3 bounds = AABB3(Vec3(0.f, -half, -half), Vec3(length, half, half));
	kBasis.Normalize();
	Vec3 jBasis = CrossProduct3D(kBasis, Vec3(1.f, 0.f, 0.f));
	Vec3 iBasis;
	if (jBasis.GetLengthSquared() == 0.f) {
		iBasis = CrossProduct3D(Vec3(0.f, 1.f, 0.f), kBasis).GetNormalized();
		jBasis = CrossProduct3D(kBasis, iBasis).GetNormalized();
	}
	else {
		iBasis = CrossProduct3D(jBasis, kBasis).GetNormalized();
		jBasis = jBasis.GetNormalized();
	}
	std::vector<Vertex_PCU> tempVerts;
	//AddVertsForAABB3D(tempVerts, bounds, color);
	AddVertsForCylinderZ3D(tempVerts, Vec2(0.f, 0.f), FloatRange(0.f, (end - start).GetLength()), radius, numofSlice, color, UVs);
	TransformVertexArrayXYZ3D((int)tempVerts.size(), tempVerts.data(), iBasis, jBasis, kBasis, start);

	verts.reserve(verts.size() + tempVerts.size());
	for (int vertsIndex = 0; vertsIndex < (int)tempVerts.size(); vertsIndex++) {
		verts.push_back(tempVerts[vertsIndex]);
	}
}

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	//int NUM_TRIS = 2 * numSlices * 3;
	//int NUM_VERTS = 2 * NUM_TRIS;
	//verts.reserve(verts.size() + NUM_VERTS);
	//float numofSlice = (float)numSlices;
	//float degreePerSlice = 360.f / numofSlice;
	//float uPerSlice = UVs.GetDimensions().x / numofSlice;

	//for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
	//	float startDegree = (float)(sliceIndex)*degreePerSlice;
	//	float endDegree = (float)(sliceIndex + 1) * degreePerSlice;
	//	Vec3 startVert = Vec3(Vec2::MakeFromPolarDegrees(startDegree, radius));
	//	Vec3 endVert = Vec3(Vec2::MakeFromPolarDegrees(endDegree, radius));
	//	Vec3 BL = startVert + start;
	//	Vec3 BR = endVert + start;

	//	Vec2 centerUV = UVs.GetCenter();
	//	Vec2 uvStart = Vec2::MakeFromPolarDegrees(startDegree, centerUV.y) + centerUV;
	//	Vec2 uvEnd = Vec2::MakeFromPolarDegrees(endDegree, centerUV.y) + centerUV;
	//	AddVertsForQuad3D(verts, BL, BR, end, end, color, uvStart, uvEnd, centerUV, centerUV);
	//	uvStart.y = UVs.m_maxs.y - uvStart.y;
	//	uvEnd.y = UVs.m_maxs.y - uvEnd.y;
	//	//AddVertsForQuad3D(verts, BR, BL, bottomCenter, bottomCenter, color, uvEnd, uvStart , Vec2(0.5f, 0.5f), Vec2(0.5f, 0.5f));
	//	AddVertsForQuad3D(verts, start, start, BR, BL, color, centerUV, centerUV, uvEnd, uvStart);
	//}

	//float numofSlice = (float)numSlices;
	Vec3 kBasis = end - start;
	//float length = kBasis.GetLength();
	//AABB3 bounds = AABB3(Vec3(0.f, -half, -half), Vec3(length, half, half));
	kBasis.Normalize();
	Vec3 jBasis = CrossProduct3D(kBasis, Vec3(1.f, 0.f, 0.f));
	Vec3 iBasis;
	if (jBasis.GetLengthSquared() == 0.f) {
		iBasis = CrossProduct3D(Vec3(0.f, 1.f, 0.f), kBasis).GetNormalized();
		jBasis = CrossProduct3D(kBasis, iBasis).GetNormalized();
	}
	else {
		iBasis = CrossProduct3D(jBasis, kBasis).GetNormalized();
		jBasis = jBasis.GetNormalized();
	}
	std::vector<Vertex_PCU> tempVerts;
	//AddVertsForAABB3D(tempVerts, bounds, color);
	//AddVertsForCylinderZ3D(tempVerts, Vec2(0.f, 0.f), FloatRange(0.f, (end - start).GetLength()), radius, numofSlice, color, UVs);
	AddVertsForConeZ3D(tempVerts, Vec2(0.f, 0.f), FloatRange(0.f, (end - start).GetLength()), radius, color, UVs, numSlices);
	TransformVertexArrayXYZ3D((int)tempVerts.size(), tempVerts.data(), iBasis, jBasis, kBasis, start);

	verts.reserve(verts.size() + tempVerts.size());
	for (int vertsIndex = 0; vertsIndex < (int)tempVerts.size(); vertsIndex++) {
		verts.push_back(tempVerts[vertsIndex]);
	}
}

void AddVertsForConeZ3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, const Rgba8& color, const AABB2& UVs, int numSlices)
{
	int NUM_TRIS = 2 * numSlices * 3;
	int NUM_VERTS = 2 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);
	float numofSlice = (float)numSlices;
	float degreePerSlice = 360.f / numofSlice;
	//float uPerSlice = UVs.GetDimensions().x / numofSlice;

	for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
		float startDegree = (float)(sliceIndex)*degreePerSlice;
		float endDegree = (float)(sliceIndex + 1) * degreePerSlice;
		Vec3 startVert = Vec3(Vec2::MakeFromPolarDegrees(startDegree, radius));
		Vec3 endVert = Vec3(Vec2::MakeFromPolarDegrees(endDegree, radius));
		Vec3 start = Vec3(centerXY);
		Vec3 end = Vec3(centerXY);
		start.z = minMaxZ.m_min;
		end.z = minMaxZ.m_max;
		Vec3 BL = startVert + start;
		Vec3 BR = endVert + start;

		Vec2 centerUV = UVs.GetCenter();
		Vec2 uvStart = Vec2::MakeFromPolarDegrees(startDegree, centerUV.y) + centerUV;
		Vec2 uvEnd = Vec2::MakeFromPolarDegrees(endDegree, centerUV.y) + centerUV;
		AddVertsForQuad3D(verts, BL, BR, end, end, color, uvStart, uvEnd, centerUV, centerUV);
		uvStart.y = UVs.m_maxs.y - uvStart.y;
		uvEnd.y = UVs.m_maxs.y - uvEnd.y;
		//AddVertsForQuad3D(verts, BR, BL, bottomCenter, bottomCenter, color, uvEnd, uvStart , Vec2(0.5f, 0.5f), Vec2(0.5f, 0.5f));
		AddVertsForQuad3D(verts, start, start, BR, BL, color, centerUV, centerUV, uvEnd, uvStart);
	}
}

void AddVertsForHexgon3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, const float& radius, const Vec3& center, const Vec3& normal, const Rgba8& color, const AABB2& UVs)
{
	UNUSED(UVs);
	Vec3 i = Vec3(1.f, 0.f, 0.f) * radius;
	Vec3 j = Vec3(0.5f, 0.866f, 0.f) * radius;
	Vec3 k = Vec3(-0.5f, 0.866f, 0.f) * radius;

	Vertex_PCUTBN verts[7];
	verts[0].m_position = center;
	verts[1].m_position = center + i;
	verts[2].m_position = center + j;
	verts[3].m_position = center + k;
	verts[4].m_position = center - i;
	verts[5].m_position = center - j;
	verts[6].m_position = center - k;

	int baseIndex = (int)vertexes.size();
	for (int index = 0; index < 7; index++) {
		verts[index].m_normal = normal;
		verts[index].m_color = color;
		vertexes.push_back(verts[index]);
		// Handle uv later
		//verts->
	}
	for (int index = 0; index < 6; index++) {
		indexes.push_back(baseIndex + 0);
		indexes.push_back(baseIndex + index + 1);
		if (index == 5)
			indexes.push_back(baseIndex + 1);
		else indexes.push_back(baseIndex + index + 2);
	}
}

void AddVertsForHexgon3D(std::vector<Vertex_PCU>& vertexes, const float& radius, const Vec3& center, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	UNUSED(UVs);
	Vec3 i = Vec3(1.f, 0.f, 0.f) * radius;
	Vec3 j = Vec3(0.5f, 0.866f, 0.f) * radius;
	Vec3 k = Vec3(-0.5f, 0.866f, 0.f) * radius;

	Vertex_PCU verts[7];
	verts[0].m_position = center;
	verts[1].m_position = center + i;
	verts[2].m_position = center + j;
	verts[3].m_position = center + k;
	verts[4].m_position = center - i;
	verts[5].m_position = center - j;
	verts[6].m_position = center - k;

	for (int index = 0; index < 7; index++) {
		verts[index].m_color = color;
	}
	for (int index = 0; index < 6; index++) {
		vertexes.push_back(verts[0]);
		vertexes.push_back(verts[index + 1]);
		if (index == 5)
			vertexes.push_back(verts[1]);
		else vertexes.push_back(verts[index + 2]);
	}
}

void AddVertsForHexgon3DFrame(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, const float& radius, const Vec3& center, const float& thickness, const Vec3& normal, const Rgba8& color, const AABB2& UVs)
{
	UNUSED(UVs);
	Vec3 inner_i = Vec3(1.f, 0.f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);
	Vec3 inner_j = Vec3(0.5f, 0.866f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);
	Vec3 inner_k = Vec3(-0.5f, 0.866f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);

	Vertex_PCUTBN innerVerts[6];

	innerVerts[0].m_position = center + inner_i;
	innerVerts[1].m_position = center + inner_j;
	innerVerts[2].m_position = center + inner_k;
	innerVerts[3].m_position = center - inner_i;
	innerVerts[4].m_position = center - inner_j;
	innerVerts[5].m_position = center - inner_k;

	Vec3 outter_i = Vec3(1.f, 0.f, 0.f) * (radius + thickness * 0.5f);
	Vec3 outter_j = Vec3(0.5f, 0.866f, 0.f) * (radius + thickness * 0.5f);
	Vec3 outter_k = Vec3(-0.5f, 0.866f, 0.f) * (radius + thickness * 0.5f);

	Vertex_PCUTBN outterVerts[6];

	outterVerts[0].m_position = center + outter_i;
	outterVerts[1].m_position = center + outter_j;
	outterVerts[2].m_position = center + outter_k;
	outterVerts[3].m_position = center - outter_i;
	outterVerts[4].m_position = center - outter_j;
	outterVerts[5].m_position = center - outter_k;

	int baseIndex = (int)vertexes.size();
	for (int index = 0; index < 6; index++) {
		innerVerts[index].m_color = color;
		innerVerts[index].m_normal = normal;
		vertexes.push_back(innerVerts[index]);
		vertexes.push_back(outterVerts[index]);
		if (index == 5) {
			indexes.push_back(baseIndex + index * 2);
			indexes.push_back(baseIndex + 1);
			indexes.push_back(baseIndex);

			indexes.push_back(baseIndex + index * 2);
			indexes.push_back(baseIndex + index * 2 + 1);
			indexes.push_back(baseIndex + 1);
			break;
		}
		indexes.push_back(baseIndex + index * 2);
		indexes.push_back(baseIndex + index * 2 + 1);
		indexes.push_back(baseIndex + index * 2 + 2);

		indexes.push_back(baseIndex + index * 2 + 3);
		indexes.push_back(baseIndex + index * 2 + 2);
		indexes.push_back(baseIndex + index * 2 + 1);

	}
}



void AddVertsForHexgon3DFrame(std::vector<Vertex_PCU>& vertexes, const float& radius, const Vec3& center, const float& thickness, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	UNUSED(UVs);
	Vec3 inner_i = Vec3(1.f, 0.f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);
	Vec3 inner_j = Vec3(0.5f, 0.866f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);
	Vec3 inner_k = Vec3(-0.5f, 0.866f, 0.f) * GetClamped((radius - thickness * 0.5f), 0.f, radius);

	Vertex_PCU innerVerts[6];

	innerVerts[0].m_position = center + inner_i;
	innerVerts[1].m_position = center + inner_j;
	innerVerts[2].m_position = center + inner_k;
	innerVerts[3].m_position = center - inner_i;
	innerVerts[4].m_position = center - inner_j;
	innerVerts[5].m_position = center - inner_k;

	Vec3 outter_i = Vec3(1.f, 0.f, 0.f) * (radius + thickness * 0.5f);
	Vec3 outter_j = Vec3(0.5f, 0.866f, 0.f) * (radius + thickness * 0.5f);
	Vec3 outter_k = Vec3(-0.5f, 0.866f, 0.f) * (radius + thickness * 0.5f);

	Vertex_PCU outterVerts[6];

	outterVerts[0].m_position = center + outter_i;
	outterVerts[1].m_position = center + outter_j;
	outterVerts[2].m_position = center + outter_k;
	outterVerts[3].m_position = center - outter_i;
	outterVerts[4].m_position = center - outter_j;
	outterVerts[5].m_position = center - outter_k;

	for (int index = 0; index < 6; index++) {
		outterVerts[index].m_color = color;
		innerVerts[index].m_color = color;
	}
	for (int index = 0; index < 6; index++) {
		if (index == 5) {
			vertexes.push_back(innerVerts[index]);
			vertexes.push_back(outterVerts[index]);
			vertexes.push_back(outterVerts[0]);

			vertexes.push_back(innerVerts[index]);
			vertexes.push_back(outterVerts[0]);
			vertexes.push_back(innerVerts[0]);
			break;
		}
		vertexes.push_back(innerVerts[index]);
		vertexes.push_back(outterVerts[index]);
		vertexes.push_back(outterVerts[index + 1]);

		vertexes.push_back(innerVerts[index]);
		vertexes.push_back(outterVerts[index + 1]);
		vertexes.push_back(innerVerts[index + 1]);
	}

	
}

void AddVertsForCube3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, const AABB3& cube, const Rgba8& color /*= Rgba8::WHITE*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	vertexes.reserve(vertexes.size() + 24);

	Vec2 uvAtMins = UVs.m_mins;
	Vec2 uvAtMaxs = UVs.m_maxs;

	float minX = cube.m_mins.x;
	float minY = cube.m_mins.y;
	float minZ = cube.m_mins.z;
	float maxX = cube.m_maxs.x;
	float maxY = cube.m_maxs.y;
	float maxZ = cube.m_maxs.z;

	// +x
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), color, UVs);
	// -x
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), color, UVs);
	// +y
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), color, UVs);
	// -y
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), color, UVs);
	// +z
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), color, UVs);
	// -z
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), color, UVs);
}

void AddVertsForSkybox(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, Mat44& modelMatrix, float scale, Vec3 center)
{
	vertexes.reserve(vertexes.size() + 24);

	float minX = -0.5f;
	float minY = -0.5f;
	float minZ = -0.5f;
	float maxX = 0.5f;
	float maxY = 0.5f;
	float maxZ = 0.5f;

	// +x
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ),  Vec3(maxX, minY, maxZ));
	// -x
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, minY, maxZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ));
	// +y
	AddVertsForQuad3D(vertexes, indexes, Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ));
	// -y
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ));
	// +z
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ));
	// -z
	AddVertsForQuad3D(vertexes, indexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ));

	EulerAngles orientation;
	modelMatrix = orientation.GetMatrix_XFwd_YLeft_ZUp();
	modelMatrix.SetTranslation3D(center);
	modelMatrix.AppendScaleUniform3D(scale);
}

void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);
	 
	Vec3 BL = Vec3(box.GetWorldPosForLocalPos(-box.m_halfDimensions));
	Vec3 BR = Vec3(box.GetWorldPosForLocalPos(Vec2(box.m_halfDimensions.x, -box.m_halfDimensions.y)));
	Vec3 TR = Vec3(box.GetWorldPosForLocalPos(box.m_halfDimensions));
	Vec3 TL = Vec3(box.GetWorldPosForLocalPos(Vec2(-box.m_halfDimensions.x, box.m_halfDimensions.y)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
}

void AddVertsForArc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float startDegree, float endDegree, Rgba8 const& color)
{
	constexpr int NUM_SIDES = 32;
	constexpr int NUM_VERTS = NUM_SIDES * 3;
	verts.reserve(verts.size() + NUM_VERTS);
	if (startDegree > endDegree)
	{
		endDegree += 360.f;
	}
	float angle = (endDegree - startDegree) / static_cast<float>(NUM_SIDES);
	for (int i = 0; i < NUM_SIDES; i++)
	{
		verts.push_back(Vertex_PCU(Vec3(center), color, Vec2(0.f, 0.f)));
		Vec2 pointOne = Vec2::MakeFromPolarDegrees(startDegree + angle * i, radius);
		verts.push_back(Vertex_PCU(Vec3(center + pointOne), color, Vec2(0.f, 0.f)));
		Vec2 pointTwo = Vec2::MakeFromPolarDegrees(startDegree + angle * (i + 1), radius);
		verts.push_back(Vertex_PCU(Vec3(center + pointTwo), color, Vec2(0.f, 0.f)));
	}
}

void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color)
{
	constexpr int NUM_SIDES = 32;
	constexpr int NUM_VERTS = NUM_SIDES * 3;
	verts.reserve(verts.size() + NUM_VERTS);
	float angle = 360.f / static_cast<float>(NUM_SIDES);
	for (int i = 0; i < NUM_SIDES; i++)
	{
		verts.push_back(Vertex_PCU(Vec3(center), color, Vec2(0.f, 0.f)));
		Vec2 pointOne = Vec2::MakeFromPolarDegrees(angle * i, radius);
		verts.push_back(Vertex_PCU(Vec3(center + pointOne), color, Vec2(0.f, 0.f)));
		Vec2 pointTwo = Vec2::MakeFromPolarDegrees(angle * (i + 1), radius);
		verts.push_back(Vertex_PCU(Vec3(center + pointTwo), color, Vec2(0.f, 0.f)));
	}
}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color)
{
	Vec2 line = capsule.m_bone.m_end - capsule.m_bone.m_start;
	Vec2 center = 0.5f * line + capsule.m_bone.m_start;
	Vec2 iBasis = (capsule.m_bone.m_end - capsule.m_bone.m_start).GetRotatedMinus90Degrees();
	iBasis.Normalize();
	Vec2 halfDimension = Vec2(capsule.m_radius, 0.5f * line.GetLength());
	OBB2 box = OBB2(center, iBasis, halfDimension);
	AddVertsForOBB2D(verts, box, color);
	float startDegree = line.GetOrientationDegrees() + 90.f;
	AddVertsForArc2D(verts, capsule.m_bone.m_start, capsule.m_radius, startDegree, startDegree + 180.f, color);
	AddVertsForArc2D(verts, capsule.m_bone.m_end, capsule.m_radius, startDegree - 180.f, startDegree, color);
}

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color)
{
	Capsule2 capsule = Capsule2(LineSegment2(boneStart, boneEnd), radius);
	AddVertsForCapsule2D(verts, capsule, color);
}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec2 iBasisNormal = (lineSegment.m_end - lineSegment.m_start).GetRotatedMinus90Degrees();
	iBasisNormal.Normalize();
	float halfThickness = 0.5f * thickness;

	Vec3 BL = Vec3(lineSegment.m_start - iBasisNormal * halfThickness);
	Vec3 BR = Vec3(lineSegment.m_start + iBasisNormal * halfThickness);
	Vec3 TR = Vec3(lineSegment.m_end + iBasisNormal * halfThickness);
	Vec3 TL = Vec3(lineSegment.m_end - iBasisNormal * halfThickness);

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));

}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	LineSegment2 line = LineSegment2(start, end);
	AddVertsForLineSegment2D(verts, line, thickness, color);
}

void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	LineSegment2 lineSegment = LineSegment2(start, end);

	Vec2 iBasisNormal = (lineSegment.m_end - lineSegment.m_start).GetRotatedMinus90Degrees();
	iBasisNormal.Normalize();
	float halfThickness = 0.5f * thickness;

	Vec3 BL = Vec3(lineSegment.m_start - iBasisNormal * halfThickness);
	Vec3 BR = Vec3(lineSegment.m_start + iBasisNormal * halfThickness);
	Vec3 TR = Vec3(lineSegment.m_end + iBasisNormal * halfThickness);
	Vec3 TL = Vec3(lineSegment.m_end - iBasisNormal * halfThickness);

	int initial = (int)verts.size();

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	indexes.push_back(initial);
	indexes.push_back(initial + 1);
	indexes.push_back(initial + 3);

	indexes.push_back(initial);
	indexes.push_back(initial + 3);
	indexes.push_back(initial + 2);
}

void AddVertsForInfiniteLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	LineSegment2 lineSegment = LineSegment2(start, end);

	Vec2 iBasisNormal = (lineSegment.m_end - lineSegment.m_start).GetRotatedMinus90Degrees();
	iBasisNormal.Normalize();
	Vec2 normalS2E = iBasisNormal.GetRotated90Degrees();
	float halfThickness = 0.5f * thickness;

	Vec3 BL = Vec3(lineSegment.m_start - normalS2E* 1000.f - iBasisNormal * halfThickness);
	Vec3 BR = Vec3(lineSegment.m_start - normalS2E * 1000.f + iBasisNormal * halfThickness);
	Vec3 TR = Vec3(lineSegment.m_end + normalS2E * 1000.f + iBasisNormal * halfThickness);
	Vec3 TL = Vec3(lineSegment.m_end + normalS2E * 1000.f - iBasisNormal * halfThickness);

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(BR, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(BL, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(TR, color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(TL, color, Vec2(0.f, 1.f)));
}

void AddVertsForLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color)
{
	float half = thickness * 0.5f;
	Vec3 iBasis = end - start;
	float length = iBasis.GetLength();
	AABB3 bounds = AABB3(Vec3(0.f, -half, -half), Vec3(length, half, half));
	iBasis.Normalize();
	Vec3 jBasis = CrossProduct3D(Vec3(0.f, 0.f, 1.f), iBasis);
	Vec3 kBasis;
	if (jBasis.GetLengthSquared() == 0.f) {
		kBasis = CrossProduct3D(iBasis, Vec3(0.f, 1.f, 0.f));
		jBasis = CrossProduct3D(kBasis, iBasis);
	}
	else {
		kBasis = CrossProduct3D(iBasis, jBasis);
	}
	std::vector<Vertex_PCU> tempVerts;
	AddVertsForAABB3D(tempVerts, bounds, color);
	TransformVertexArrayXYZ3D((int)tempVerts.size(), tempVerts.data(), iBasis, jBasis, kBasis, start);	

	verts.reserve(verts.size() + tempVerts.size());
	for (int vertsIndex = 0; vertsIndex < (int)tempVerts.size(); vertsIndex++) {
		verts.push_back(tempVerts[vertsIndex]);
	}
}

void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color)
{
	AddVertsForLineSegment2D(verts, tailPos, tipPos, lineThickness, color);

	Vec2 tipToTailNormal = (tailPos - tipPos).GetNormalized();
	Vec2 vertOne = tipPos + arrowSize * tipToTailNormal.GetRotatedDegrees(30.f);
	Vec2 vertTwo = tipPos + arrowSize * tipToTailNormal.GetRotatedDegrees(-30.f);
	verts.push_back(Vertex_PCU(tipPos, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(vertTwo, color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(vertOne, color, Vec2(0.f, 0.f)));
	
}

void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float const& radius, Rgba8 const& color)
{
	Vec3 divide = start + 0.8f * (end - start);
	AddVertsForCylinder3D(verts, start, divide, radius, color);
	AddVertsForCone3D(verts, divide, end, radius * 2, color);
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UVs)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec2 uvAtMins = UVs.m_mins;
	Vec2 uvAtMaxs = UVs.m_maxs;

	verts.emplace_back(Vertex_PCU(bottomLeft, color, uvAtMins));
	verts.emplace_back(Vertex_PCU(bottomRight, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
	verts.emplace_back(Vertex_PCU(topRight, color, uvAtMaxs));
		  
	verts.emplace_back(Vertex_PCU(bottomLeft, color, uvAtMins));
	verts.emplace_back(Vertex_PCU(topRight, color, uvAtMaxs));
	verts.emplace_back(Vertex_PCU(topLeft, color, Vec2(uvAtMins.x, uvAtMaxs.y)));
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const Vec2& uvBL, const Vec2& uvBR, const Vec2& uvTR, const Vec2& uvTL)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	verts.push_back(Vertex_PCU(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCU(bottomRight, color, uvBR));
	verts.push_back(Vertex_PCU(topRight, color, uvTR));

	verts.push_back(Vertex_PCU(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCU(topRight, color, uvTR));
	verts.push_back(Vertex_PCU(topLeft, color, uvTL));
}

void AddVertsForQuad3D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indexes, const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Rgba8& color, const AABB2& UVs)
{
	vertexes.reserve(vertexes.size() + 4);
	indexes.reserve(indexes.size() + 6);

	Vec2 uvAtMins = UVs.m_mins;
	Vec2 uvAtMaxs = UVs.m_maxs;

	vertexes.push_back(Vertex_PCU(bottomLeft, color, uvAtMins));
	vertexes.push_back(Vertex_PCU(bottomRight, color, Vec2(uvAtMaxs.x, uvAtMins.y)));
	vertexes.push_back(Vertex_PCU(topRight, color, uvAtMaxs));
	vertexes.push_back(Vertex_PCU(topLeft, color, Vec2(uvAtMins.x, uvAtMaxs.y)));

	int nowSize = (int)vertexes.size();
	indexes.push_back(nowSize + 0);
	indexes.push_back(nowSize + 1);
	indexes.push_back(nowSize + 2);
	indexes.push_back(nowSize + 0);
	indexes.push_back(nowSize + 2);
	indexes.push_back(nowSize + 3);
}
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const Vec2& uvBL, const Vec2& uvBR, const Vec2& uvTR, const Vec2& uvTL)
{
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCUTBN(bottomRight, color, uvBR));
	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR));

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR));
	verts.push_back(Vertex_PCUTBN(topLeft, color, uvTL));
}

void AddVertsForQuad3D_PNCU(std::vector<Vertex_PCUTBN>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UVs)
{
	Vec2 uvBL = UVs.m_mins;
	Vec2 uvBR = Vec2(UVs.m_maxs.x, UVs.m_mins.y);
	Vec2 uvTL = Vec2(UVs.m_mins.x, UVs.m_maxs.y);
	Vec2 uvTR = UVs.m_maxs;
	
	constexpr int NUM_TRIS = 2;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCUTBN(bottomRight, color, uvBR));
	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR));

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL));
	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR));
	verts.push_back(Vertex_PCUTBN(topLeft, color, uvTL));
}
//void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
//	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE)
//{
//	Vec2 uvBL = UVs.m_mins;
//	Vec2 uvBR = Vec2(UVs.m_maxs.x, UVs.m_mins.y);
//	Vec2 uvTL = Vec2(UVs.m_mins.x, UVs.m_maxs.y);
//	Vec2 uvTR = UVs.m_maxs;
//
//	constexpr int NUM_TRIS = 2;
//	constexpr int NUM_VERTS = 2 * NUM_TRIS;
//	verts.reserve(verts.size() + NUM_VERTS);
//	indexes.reserve(indexes.size() + 6);
//
//	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL));
//	verts.push_back(Vertex_PCUTBN(bottomRight, color, uvBR));
//	verts.push_back(Vertex_PCUTBN(topLeft, color, uvTL));
//	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR));
//
//	int base = (int)indexes.size();
//	indexes.push_back(base);
//	indexes.push_back(base + 1);
//	indexes.push_back(base + 3);
//	indexes.push_back(base);
//	indexes.push_back(base + 3);
//	indexes.push_back(base + 2);
//}
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Rgba8& color, const AABB2& UVs)
{
	vertexes.reserve(vertexes.size() + 4);
	indexes.reserve(indexes.size() + 6);

	Vec2 uvAtMins = UVs.m_mins;
	Vec2 uvAtMaxs = UVs.m_maxs;

	Vec3 normal = CrossProduct3D((bottomRight - bottomLeft), (topLeft - bottomLeft)).GetNormalized();
	int nowSize = (int)vertexes.size();
	vertexes.push_back(Vertex_PCUTBN(bottomLeft, color, uvAtMins, normal));
	vertexes.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(uvAtMaxs.x, uvAtMins.y), normal));
	vertexes.push_back(Vertex_PCUTBN(topRight, color, uvAtMaxs, normal));
	vertexes.push_back(Vertex_PCUTBN(topLeft, color, Vec2(uvAtMins.x, uvAtMaxs.y), normal));

	//int nowSize = (int)vertexes.size();
	indexes.push_back(nowSize + 0);
	indexes.push_back(nowSize + 1);
	indexes.push_back(nowSize + 2);
	indexes.push_back(nowSize + 0);
	indexes.push_back(nowSize + 2);
	indexes.push_back(nowSize + 3);
}

void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, const Vec3& topLeft, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Rgba8& color, const AABB2& UVs)
{
	constexpr int NUM_TRIS = 4;
	constexpr int NUM_VERTS = 3 * NUM_TRIS;
	verts.reserve(verts.size() + NUM_VERTS);

	Vec2 uvBL = UVs.m_mins;
	Vec2 uvTL = Vec2(UVs.m_mins.x, UVs.m_maxs.y);
	Vec2 uvBR = Vec2(UVs.m_maxs.x, UVs.m_mins.y);
	Vec2 uvTR = UVs.m_maxs;
	Vec2 uvCT = Vec2(uvBL.x + UVs.GetDimensions().x * 0.5f, uvTR.y);
	Vec2 uvCB = Vec2(uvBL.x + UVs.GetDimensions().x * 0.5f, uvBL.y);

	// can change to pivot x
	Vec3 centerBottom = bottomLeft + 0.5f * (bottomRight - bottomLeft);
	Vec3 centerTop = topLeft + 0.5f * (topRight - topLeft);

	Vec3 normal = CrossProduct3D((bottomRight - bottomLeft), (topLeft - bottomLeft)).GetNormalized();
	Vec3 u = (bottomRight - bottomLeft).GetNormalized();

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, uvBL, -u));
	verts.push_back(Vertex_PCUTBN(centerBottom, color, uvCB, normal));
	verts.push_back(Vertex_PCUTBN(topLeft, color, uvTL, -u));

	verts.push_back(Vertex_PCUTBN(centerBottom, color, uvCB, normal));
	verts.push_back(Vertex_PCUTBN(centerTop, color, uvCT, normal));
	verts.push_back(Vertex_PCUTBN(topLeft, color, uvTL, -u));

	verts.push_back(Vertex_PCUTBN(centerBottom, color, uvCB, normal));
	verts.push_back(Vertex_PCUTBN(bottomRight, color, uvBR, u));
	verts.push_back(Vertex_PCUTBN(centerTop, color, uvCT, normal));

	verts.push_back(Vertex_PCUTBN(bottomRight, color, uvBR, u));
	verts.push_back(Vertex_PCUTBN(topRight, color, uvTR, u));
	verts.push_back(Vertex_PCUTBN(centerTop, color, uvCT, normal));
	
}
