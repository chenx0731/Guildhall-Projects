#include "Engine/Math/RaycastResult2D.hpp"
RaycastResult2D::RaycastResult2D()
{
}

RaycastResult2D::RaycastResult2D(Vec2 impactPosition, bool didImpact, float impactDistance)
{
	m_impactPos = impactPosition;
	m_didImpact = didImpact;
	m_impactDis = impactDistance;
}

RaycastResult2D::RaycastResult2D(Vec2 impactPosition, bool didImpact, float impactDistance, Vec2 impactNormal)
{
	m_impactPos = impactPosition;
	m_didImpact = didImpact;
	m_impactDis = impactDistance;
	m_impactNormal = impactNormal;
}

RaycastResult2D::RaycastResult2D(Vec2 impactPos, bool didImpact, float impactDis, Vec2 impactNormal, Vec2 rayFwdNormal, Vec2 rayStartPos, float rayMaxLength)
{
	m_impactPos = impactPos;
	m_didImpact = didImpact;
	m_impactDis = impactDis;
	m_impactNormal = impactNormal;
	m_rayFwdNormal = rayFwdNormal;
	m_rayMaxLength = rayMaxLength;
	m_rayStartPos = rayStartPos;
}
