#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"

#include <vector>
//struct Vec2;
//struct Vec3;
//struct AABB2;
//struct AABB3;
struct FloatRange;
struct OBB2;
struct Capsule2;
struct LineSegment2;
struct ConvexPoly2D;
struct Mat44;
//struct Vertex_PNCU;

void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float scaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
//void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float scaleXY, float rotationDegreesAboutZ, Vec3 const& translationXYZ);
void TransformVertexArrayXYZ3D(int numVerts, Vertex_PCU* verts, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation);
void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, const Mat44& transform);
void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, const Mat44& transform);
void CalculateTangentSpaceBasisVectors(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes);

void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color);
void AddVertsForAABB2DWithPaddings(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, 
	float top = 0.f, float bottom = 0.f, float left = 0.f, float right = 0.f);
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs);
void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color);
void AddVertsForArc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float startDegree, float endDegree, Rgba8 const& color);
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color);
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color);
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForInfiniteLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color);
void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color);
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float const& radius, Rgba8 const& color);
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const Vec2& uvBL, const Vec2& uvBR, const Vec2& uvTR, const Vec2& uvTL);
void AddVertsForQuad3D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indexes,
	const Vec3& topLeft, const Vec3& bottomLeft,
	const Vec3& bottomRight, const Vec3& topRight,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const Vec2& uvBL, const Vec2& uvBR, const Vec2& uvTR, const Vec2& uvTL);
void AddVertsForQuad3D_PNCU(std::vector<Vertex_PCUTBN>& verts,
	const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
//void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes,
//	const Vec3& topLeft, const Vec3& bottomLeft,
//	const Vec3& bottomRight, const Vec3& topRight,
//	const Rgba8& color = Rgba8::WHITE,
//	const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& vertexes, 
	const Vec3& topLeft, const Vec3& bottomLeft, 
	const Vec3& bottomRight, const Vec3& topRight, 
	const Rgba8& color = Rgba8::WHITE, 
	const AABB2& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForAABBWireframe3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, float lineThickness, Rgba8 const& tint = Rgba8::WHITE);
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, const Vec3& center, float radius,
	const Rgba8& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE, int numLatitudePieces = 8);
void AddVertsForCylinderZ3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices,
	Rgba8 const& color = Rgba8::WHITE, AABB2 const& UV = AABB2::ZERO_TO_ONE);
void AddVertsForCylinderZWireframe3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices, float lineThickness,
	Rgba8 const& color = Rgba8::WHITE);
void AddVertsForUVSphereZ3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForUVSphereZWireframe3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices, float numStacks, float lineThickness, Rgba8 const& color = Rgba8::WHITE);
//void AddVertsForSphereWireframe3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float num)

AABB2 GetVertexBounds2D(const std::vector<Vertex_PCU>& verts);

void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts,
	const Vec3& start, const Vec3& end, float radius,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE,
	int numSlices = 8);

void AddVertsForCone3D(std::vector<Vertex_PCU>& verts,
	const Vec3& start, const Vec3& end, float radius,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE,
	int numSlices = 8);

void AddVertsForConeZ3D(std::vector<Vertex_PCU>& verts,
	Vec2 const& centerXY, FloatRange const& minMaxZ, float radius,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE,
	int numSlices = 8);

void AddVertsForHexgon3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes,
	const float& radius, const Vec3& center,
	const Vec3& normal,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForHexgon3DFrame(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes,
	const float& radius, const Vec3& center,
	const float& thickness,
	const Vec3&	normal,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForHexgon3D(std::vector<Vertex_PCU>& vertexes,
	const float& radius, const Vec3& center,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForHexgon3DFrame(std::vector<Vertex_PCU>& vertexes,
	const float& radius, const Vec3& center,
	const float& thickness,
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForCube3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, 
	const AABB3& cube, 
	const Rgba8& color = Rgba8::WHITE,
	const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForSkybox(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, Mat44& modelMatrix, 
	float scale, Vec3 center);

void AddVertsForUVSphereZ3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, 
	Vec3 const& center, float radius, float numSlices, float numStacks, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

void AddVertsForTBNDebug(std::vector<Vertex_PCU>& vertexes, const std::vector<Vertex_PCUTBN>& vertsTBN);

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, const ConvexPoly2D& convexPoly, Rgba8 color = Rgba8::WHITE);

void AddVertsForConvexPoly2DFrame(std::vector<Vertex_PCU>& verts, const ConvexPoly2D& convexPoly, float thickness, Rgba8 color = Rgba8::WHITE);

void AddVertsForConvexPoly2D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const ConvexPoly2D& convexPoly, Rgba8 color = Rgba8::WHITE);

void AddVertsForConvexPoly2DFrame(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, const ConvexPoly2D& convexPoly, float thickness, Rgba8 color = Rgba8::WHITE);