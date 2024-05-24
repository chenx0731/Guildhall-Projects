#include "Engine/Math/LineSegment2.hpp"

LineSegment2::LineSegment2()
{
}

LineSegment2::LineSegment2(Vec2 start, Vec2 end)
{
	m_start = start;
	m_end = end;
}

void LineSegment2::Translate(Vec2 translation)
{
	m_start += translation;
	m_end	+= translation;
}

void LineSegment2::SetCenter(Vec2 newCenter)
{
	Vec2 halfLine = (m_end - m_start) * 0.5f;
	m_end = newCenter + halfLine;
	m_start = newCenter - halfLine;
}

void LineSegment2::RotateAboutCenter(float rotationDeltaDegrees)
{
	Vec2 halfLine = (m_end - m_start) * 0.5f;
	Vec2 center = halfLine;
	halfLine.RotateDegrees(rotationDeltaDegrees);
	m_start = -halfLine;
	m_end = halfLine;
	SetCenter(center);
}
