#include "Engine/Math/ConvexHull2D.hpp"
#include "Engine/Math/ConvexPoly2D.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/BufferWriter.hpp"

ConvexHull2D::ConvexHull2D()
{

}

ConvexHull2D::ConvexHull2D(std::vector<Plane2D> planes)
{
	m_planes = planes;
}

ConvexHull2D::ConvexHull2D(ConvexPoly2D convexPoly2D)
{
	std::vector<Vec2> points = convexPoly2D.GetAllPoints();
	if (int(points.size()) < 2) {
		return;
	}
	std::vector<Plane2D> planes;
	for (int pointIndex = 0; pointIndex < (int)points.size(); pointIndex++) {
		Vec2 pointA = points[pointIndex];
		Vec2 pointB;
		if (pointIndex != (int)points.size() - 1)
			pointB = points[pointIndex + 1];
		else pointB = points[0];
		Vec2 normal = (pointB - pointA).GetRotatedMinus90Degrees();
		float distFromOrigin = DotProduct2D(pointA, normal);
		Plane2D plane = Plane2D(normal, distFromOrigin);
		planes.push_back(plane);
	}
	m_planes = planes;  
}

RaycastResult2D ConvexHull2D::RaycastVsConvexHull2D(Vec2 start, Vec2 normal, float maxDist) const
{
	RaycastResult2D enter;
	RaycastResult2D exit;
	RaycastResult2D res;
	float lastEnter = 0.f;
	float firstExit = maxDist;

	Vec2 endR = start + normal * maxDist;
	if (IsPointInside(start)) {
		enter.m_didImpact = true;
		enter.m_impactDis = 0.f;
		enter.m_impactPos = start;
		return enter;
	}
	
	for (int planeIndex = 0; planeIndex < (int)m_planes.size(); planeIndex++) {
		RaycastResult2D temp = m_planes[planeIndex].RaycastVsPlane2(start, normal, maxDist);
		if (!temp.m_didImpact)
			continue;
		if (temp.m_impactNormal == m_planes[planeIndex].m_normal) {
			if (temp.m_impactDis > lastEnter) {
				enter = temp;
				lastEnter = temp.m_impactDis;
			}
		}
		else {
			if (temp.m_impactDis < firstExit) {
				exit = temp;
				firstExit = temp.m_impactDis;
			}
		}
	}

	// if no enter
	if (!enter.m_didImpact)
		return res;

	// if exit before enter, then miss
	if (firstExit < lastEnter) {
		return res;
	}

	if (!exit.m_didImpact) {
		if (IsPointInside(endR)) {
			return enter;
		}
		else return res;
	}

	Vec2 midPoint = 0.5f * (enter.m_impactPos + exit.m_impactPos);
	if (IsPointInside(midPoint)) {
		enter.m_exitDis = exit.m_impactDis;
		return enter;
	}
	else return res;
}

void ConvexHull2D::WriteToBuffer(BufferWriter& bufWriter)
{
	uint16_t num = uint16_t(m_planes.size());
	bufWriter.AppendUint16(num);
	for (int i = 0; i < num; i++) {
		bufWriter.AppendVec2(m_planes[i].m_normal);
		bufWriter.AppendFloat(m_planes[i].m_distFromOrigin);
	}
}

void ConvexHull2D::ParseFromBuffer(BufferParser& bufParser)
{
	m_planes.clear();
	uint16_t num = bufParser.ParseUint16();
	m_planes.reserve(num);
	for (uint16_t i = 0; i < num; i++) {
		Plane2D plane;
		plane.m_normal = bufParser.ParseVec2();
		plane.m_distFromOrigin = bufParser.ParseFloat();
		m_planes.push_back(plane);
	}
}

bool ConvexHull2D::IsPointInside(Vec2 point) const
{
	for (int planeIndex = 0; planeIndex < (int)m_planes.size(); planeIndex++) {
		float altitude = m_planes[planeIndex].GetAltitude(point);
		if (altitude >= 0.f)
			return false;
	}
	return true;
}
