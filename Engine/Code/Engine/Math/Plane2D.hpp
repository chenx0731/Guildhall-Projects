#pragma once
#include "Engine/Math/Vec2.hpp"
struct RaycastResult2D;
struct Plane2D
{
	Vec2 m_normal = Vec2();
	float m_distFromOrigin = 0.f;

	Plane2D();
	Plane2D(Vec2 normal, float distFromOrigin);
	
	float GetAltitude(Vec2 point) const;
	Vec2 GetNearestPoint(Vec2 point) const;

	RaycastResult2D RaycastVsPlane2(Vec2 start, Vec2 normal, float maxDist) const;
};