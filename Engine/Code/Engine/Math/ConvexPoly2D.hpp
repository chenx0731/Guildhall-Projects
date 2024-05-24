#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>
struct RaycastResult2D;
class BufferParser;
class BufferWriter;
struct ConvexPoly2D
{
private:
	std::vector<Vec2> m_points;
public:
	ConvexPoly2D();
	ConvexPoly2D(BufferParser& bufferParser);
	ConvexPoly2D(std::vector<Vec2> CCWOrderedPoints);
	bool IsPointInside(Vec2 point) const;
	Vec2 GetNearestPoint(Vec2 point) const;
	std::vector<Vec2> GetAllPoints() const;

	RaycastResult2D RaycastVsConvexPoly(Vec2 start, Vec2 normal, float maxDist) const;

	void Translate(Vec2 translation);
	void RotateAboutPoint(Vec2 point, float degrees);
	void ScaleAboutPoint(Vec2 point, float scale);

	void WriteToBuffer(BufferWriter& bufferWriter);
	void ParseFromBuffer(BufferParser& bufferParser);
};