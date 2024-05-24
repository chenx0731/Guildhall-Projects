#pragma once
#include "Engine/Math/Plane2D.hpp"

#include <vector>

struct ConvexPoly2D;
struct RaycastResult2D;

class BufferWriter;
class BufferParser;

struct ConvexHull2D
{
public:
	ConvexHull2D();
	ConvexHull2D(std::vector<Plane2D> planes);
	ConvexHull2D(ConvexPoly2D convexPoly2D);

	RaycastResult2D RaycastVsConvexHull2D(Vec2 start, Vec2 normal, float maxDist) const;

	void WriteToBuffer(BufferWriter& bufWriter);

	void ParseFromBuffer(BufferParser& bufParser);

	bool IsPointInside(Vec2 point) const;

public:
	std::vector<Plane2D> m_planes;
};