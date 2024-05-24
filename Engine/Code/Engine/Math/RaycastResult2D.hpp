#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
struct RaycastResult2D
{
public:
	RaycastResult2D();
	RaycastResult2D(Vec2 impactPosition, bool didImpact, float impactDistance);
	RaycastResult2D(Vec2 impactPosition, bool didImpact, float impactDistance, Vec2 impactNormal);
	RaycastResult2D(Vec2 impactPos, bool didImpact, float impactDis, Vec2 impactNormal, Vec2 rayFwdNormal, Vec2 rayStartPos, float rayMaxLength);
public:
	bool m_didImpact = false;
	float m_impactDis = 0.f;
	float m_exitDis = 0.f;
	Vec2 m_impactPos;
	Vec2 m_impactNormal;

	Vec2 m_rayFwdNormal;
	Vec2 m_rayStartPos;
	float m_rayMaxLength = 1.f;
};

struct RaycastResult3D
{
public:
	bool m_didImpact = false;
	float m_impactDis = 0.f;
	float m_exitDis = 0.f;
	Vec3 m_impactPos;
	Vec3 m_impactNormal;

	Vec3 m_rayFwdNormal;
	Vec3 m_rayStartPos;
	float m_rayMaxLength = 1.f;
};