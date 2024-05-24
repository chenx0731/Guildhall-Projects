#include "Engine/Math/AABB2.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(0.f, 0.f, 1.f, 1.f);

AABB2::AABB2(AABB2 const& copyForm)
{
	m_mins = copyForm.m_mins;
	m_maxs = copyForm.m_maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	m_mins.x = minX;
	m_mins.y = minY;
	m_maxs.x = maxX;
	m_maxs.y = maxY;
}

AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}

bool AABB2::IsPointInside(Vec2 const& point) const
{
	if (point.x <= m_maxs.x &&
		point.y <= m_maxs.y &&
		point.x >= m_mins.x &&
		point.y >= m_mins.y
		)
	{
		return true;
	}
	return false;
}

Vec2 const AABB2::GetCenter() const
{
	return 0.5f * (m_mins + m_maxs);
}

Vec2 const AABB2::GetDimensions() const
{
	return m_maxs - m_mins;
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const
{
	float returnX = 0.0f;
	float returnY = 0.0f;
	if (referencePosition.x > m_maxs.x)
	{
		returnX = m_maxs.x;
	}
	else if(referencePosition.x < m_mins.x)
	{
		returnX = m_mins.x;
	}
	else
	{
		returnX = referencePosition.x;
	}
	if (referencePosition.y > m_maxs.y)
	{
		returnY = m_maxs.y;
	}
	else if (referencePosition.y < m_mins.y)
	{
		returnY = m_mins.y;
	}
	else
	{
		returnY = referencePosition.y;
	}

	return Vec2(returnX, returnY);
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return Vec2(uv.x *(m_maxs.x - m_mins.x) + m_mins.x, uv.y * (m_maxs.y - m_mins.y) + m_mins.y);
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	return Vec2((point.x - m_mins.x) / (m_maxs.x - m_mins.x), (point.y - m_mins.y) / (m_maxs.y - m_mins.y));;
}

Vec2 const AABB2::GetBottomRight() const
{
	return Vec2(m_maxs.x, m_mins.y);
}

Vec2 const AABB2::GetTopLeft() const
{
	return Vec2(m_mins.x, m_maxs.y);
}

void AABB2::Translate(Vec2 const& translationToApply)
{
	m_mins += translationToApply;
	m_maxs += translationToApply;
}

void AABB2::SetCenter(Vec2 const& newCenter)
{
	Vec2 half = 0.5f * (m_maxs - m_mins);
	m_mins = newCenter - half;
	m_maxs = newCenter + half;
}

void AABB2::SetDimensions(Vec2 const& newDimensions)
{
	Vec2 center = 0.5f * (m_mins + m_maxs);
	m_mins = center - 0.5f * newDimensions;
	m_maxs = center + 0.5f * newDimensions;
}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{
	float max_X = m_maxs.x;
	float min_X = m_mins.x;
	float max_Y = m_maxs.y;
	float min_Y = m_mins.y;
	if (point.x > max_X)
	{
		max_X = point.x;
	}
	else if(point.x < min_X)
	{
		min_X = point.x;
	}
	if (point.y > max_Y)
	{
		max_Y = point.y;
	}
	else if (point.y < min_Y)
	{
		min_Y = point.y;
	}
	m_mins = Vec2(min_X, min_Y);
	m_maxs = Vec2(max_X, max_Y);
}
