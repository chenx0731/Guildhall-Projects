#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RaycastResult2D.hpp"

Plane2D::Plane2D()
{
}



Plane2D::Plane2D(Vec2 normal, float distFromOrigin)
{
	m_normal = normal;
	m_distFromOrigin = distFromOrigin;
}

float Plane2D::GetAltitude(Vec2 point) const
{
	return DotProduct2D(point, m_normal) - m_distFromOrigin;
}

Vec2 Plane2D::GetNearestPoint(Vec2 point) const
{
	Vec2 nearest = point - GetAltitude(point) * m_normal;
	return nearest;
}

RaycastResult2D Plane2D::RaycastVsPlane2(Vec2 start, Vec2 normal, float maxDist) const
{
	RaycastResult2D res;
	Vec2 endR = start + normal * maxDist;
	float altS = GetAltitude(start);
	float altE = GetAltitude(endR);
	if (altS * altE >= 0.f)
		return res;
	res.m_didImpact = true;
	float normalProjectToPN = DotProduct2D(normal, m_normal);
	res.m_impactDis = -(altS / normalProjectToPN);
	res.m_impactPos = start + normal * res.m_impactDis;
	if (altS > 0.f)
		res.m_impactNormal = m_normal;
	else res.m_impactNormal = -m_normal;
	return res;
}

