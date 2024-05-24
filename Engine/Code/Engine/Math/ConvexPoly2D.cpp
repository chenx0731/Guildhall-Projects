#include "Engine/Math/ConvexPoly2D.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"
ConvexPoly2D::ConvexPoly2D(std::vector<Vec2> CCWOrderedPoints)
{
	m_points = CCWOrderedPoints;
}

ConvexPoly2D::ConvexPoly2D()
{

}


ConvexPoly2D::ConvexPoly2D(BufferParser& bufferParser)
{
	uint32_t size = bufferParser.ParseUint32();
	for (uint32_t i = 0; i < size; i++)
	{
		Vec2 point = bufferParser.ParseVec2();
		m_points.push_back(point);
	}
}

bool ConvexPoly2D::IsPointInside(Vec2 point) const
{
	for (int pointIndex = 0; pointIndex < (int)m_points.size(); pointIndex++) {
		Vec2 startP = m_points[pointIndex];
		Vec2 endP;
		if (pointIndex == (int)m_points.size() - 1)
			endP = m_points[0];
		else endP = m_points[pointIndex + 1];

		Vec2 sToE = endP - startP;
		Vec2 sToP = point - startP;
		if (CrossProduct2D(sToE, sToP) <= 0.f) {
			return false;
		}
	}
	return true;
}

Vec2 ConvexPoly2D::GetNearestPoint(Vec2 point) const
{
	return Vec2();
}

std::vector<Vec2> ConvexPoly2D::GetAllPoints() const
{
	std::vector<Vec2> points = m_points;
	return points;
}

RaycastResult2D ConvexPoly2D::RaycastVsConvexPoly(Vec2 start, Vec2 normal, float maxDist) const
{
	RaycastResult2D res;
	//float minImpact = maxDist;
	if (IsPointInside(start)) {
		res.m_didImpact = true;
		res.m_impactDis = 0.f;
		res.m_impactPos = start;
		return res;
	}
	for (int pointIndex = 0; pointIndex < (int)m_points.size(); pointIndex++) {
		Vec2 startL = m_points[pointIndex];
		Vec2 endL;
		if (pointIndex == (int)m_points.size() - 1)
			endL = m_points[0];
		else endL = m_points[pointIndex + 1];
		RaycastResult2D temp;
		temp = RaycastVsLine2D(start, normal, maxDist, LineSegment2(startL, endL));
		if (!temp.m_didImpact) 
			continue;
		if (temp.m_impactDis < maxDist) {
			res = temp;
		}
	}
	return res;
}

void ConvexPoly2D::Translate(Vec2 translation)
{
	for (int pointIndex = 0; pointIndex < (int)m_points.size(); pointIndex++) {
		m_points[pointIndex] += translation;
	}
}

void ConvexPoly2D::RotateAboutPoint(Vec2 point, float degrees)
{
	for (int i = 0; i < (int)m_points.size(); i++)
	{
		Vec2 pointToVert = m_points[i] - point;
		pointToVert.RotateDegrees(degrees);
		m_points[i] = point + pointToVert;
	}

}

void ConvexPoly2D::ScaleAboutPoint(Vec2 point, float scale)
{
	for (int i = 0; i < (int)m_points.size(); i++)
	{
		Vec2 pointToVert = m_points[i] - point;
		pointToVert *= scale;
		m_points[i] = point + pointToVert;
	}
}




void ConvexPoly2D::WriteToBuffer(BufferWriter& bufferWriter)
{
	uint16_t size = (uint16_t)m_points.size();
	bufferWriter.AppendUint16(size);
	for (uint16_t i = 0; i < size; i++) {
		bufferWriter.AppendVec2(m_points[i]);
	}
}

void ConvexPoly2D::ParseFromBuffer(BufferParser& bufferParser)
{
	m_points.clear();
	uint16_t size = bufferParser.ParseUint16();
	m_points.reserve(size);
	for(uint16_t i = 0; i < size; i++) {
		Vec2 temp = bufferParser.ParseVec2();
		m_points.push_back(temp);
	}
}
