#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"

const AABB3 AABB3::ZERO_TO_ONE = AABB3(0.f, 0.f ,0.f, 1.f, 1.f, 1.f);

AABB3::AABB3(AABB3 const& copyForm)
{
	m_mins = copyForm.m_mins;
	m_maxs = copyForm.m_maxs;
}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_mins.x = minX;
	m_mins.y = minY;
	m_maxs.x = maxX;
	m_maxs.y = maxY;
	m_mins.z = minZ;
	m_maxs.z = maxZ;
}

AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
{
	m_mins = mins;
	m_maxs = maxs;
}

bool AABB3::IsPointInside(Vec3 const& point) const
{
	if (point.x < m_maxs.x &&
		point.y < m_maxs.y &&
		point.z < m_maxs.z &&
		point.x > m_mins.x &&
		point.y > m_mins.y &&
		point.z > m_mins.z
		)
	{
		return true;
	}
	return false;
}

Vec3 const AABB3::GetCenter() const
{
	return 0.5f * (m_mins + m_maxs);
}

Vec3 const AABB3::GetDimensions() const
{
	return m_maxs - m_mins;
}

Vec3 const AABB3::GetNearestPoint(Vec3 const& referencePosition) const
{
	float returnX = 0.0f;
	float returnY = 0.0f;
	float returnZ = 0.0f;
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
	if (referencePosition.z > m_maxs.z)
	{
		returnZ = m_maxs.z;
	}
	else if (referencePosition.z < m_mins.z)
	{
		returnZ = m_mins.z;
	}
	else
	{
		returnZ = referencePosition.z;
	}
	return Vec3(returnX, returnY, returnZ);
}

Vec3 const AABB3::GetPointAtUV(Vec3 const& uv) const
{
	return Vec3(uv.x *(m_maxs.x - m_mins.x) + m_mins.x, uv.y * (m_maxs.y - m_mins.y) + m_mins.y, uv.z * (m_maxs.z - m_mins.z) + m_mins.z);
}

Vec3 const AABB3::GetUVForPoint(Vec3 const& point) const
{
	return Vec3((point.x - m_mins.x) / (m_maxs.x - m_mins.x), (point.y - m_mins.y) / (m_maxs.y - m_mins.y), (point.z - m_mins.z) / (m_maxs.z - m_mins.z));;
}

void AABB3::Translate(Vec3 const& translationToApply)
{
	m_mins += translationToApply;
	m_maxs += translationToApply;
}

void AABB3::SetCenter(Vec3 const& newCenter)
{
	Vec3 half = 0.5f * (m_maxs - m_mins);
	m_mins = newCenter - half;
	m_maxs = newCenter + half;
}

void AABB3::SetDimensions(Vec3 const& newDimensions)
{
	Vec3 center = 0.5f * (m_mins + m_maxs);
	m_mins = center - 0.5f * newDimensions;
	m_maxs = center + 0.5f * newDimensions;
}

void AABB3::StretchToIncludePoint(Vec3 const& point)
{
	float max_X = m_maxs.x;
	float min_X = m_mins.x;
	float max_Y = m_maxs.y;
	float min_Y = m_mins.y;
	float max_Z = m_maxs.z;
	float min_Z = m_mins.z;
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
	if (point.z > max_Z)
	{
		max_Z = point.z;
	}
	else if (point.z < min_Z)
	{
		min_Z = point.z;
	}
	m_mins = Vec3(min_X, min_Y, min_Z);
	m_maxs = Vec3(max_X, max_Y, max_Z);
}

AABB2 AABB3::GetAABB2XY() const
{
	return AABB2(m_mins.x, m_mins.y, m_maxs.x, m_maxs.y);
}
