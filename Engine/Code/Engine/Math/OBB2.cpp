#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"

OBB2::OBB2()
{
}

OBB2::OBB2(Vec2 center, Vec2 iBasisNormal, Vec2 halfDimensions)
{
	m_center = center;
	m_iBasisNormal = iBasisNormal;
	m_halfDimensions = halfDimensions;
}

void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const
{
	Vec2 SL = GetWorldPosForLocalPos(-m_halfDimensions);
	Vec2 SR = GetWorldPosForLocalPos(Vec2(m_halfDimensions.x, -m_halfDimensions.y));
	Vec2 EL = GetWorldPosForLocalPos(Vec2(-m_halfDimensions.x, m_halfDimensions.y));
	Vec2 ER = GetWorldPosForLocalPos(m_halfDimensions);
	out_fourCornerWorldPositions[0] = SL;
	out_fourCornerWorldPositions[1] = SR;
	out_fourCornerWorldPositions[2] = EL;
	out_fourCornerWorldPositions[3] = ER;
}

Vec2 OBB2::GetLocalPosForWorldPos(Vec2 worldPos) const
{
	Vec2 jBasis = m_iBasisNormal.GetRotated90Degrees();
	float localX = DotProduct2D(worldPos - m_center, m_iBasisNormal);
	float localY = DotProduct2D(worldPos - m_center, jBasis);
	return Vec2(localX, localY);
}

Vec2 OBB2::GetWorldPosForLocalPos(Vec2 localPos) const
{
	Vec2 jBasis = m_iBasisNormal.GetRotated90Degrees();
	return m_center + localPos.x * m_iBasisNormal + localPos.y * jBasis;
}

void OBB2::RotateAboutCenter(float rotationDeltaDegrees)
{
	m_iBasisNormal.RotateDegrees(rotationDeltaDegrees);
}
