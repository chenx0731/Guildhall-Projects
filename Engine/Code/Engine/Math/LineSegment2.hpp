#pragma once
#include "Engine/Math/Vec2.hpp"
struct LineSegment2
{
	Vec2 m_start;
	Vec2 m_end;

	LineSegment2();
	LineSegment2(Vec2 start, Vec2 end);
	void Translate(Vec2 translation);
	void SetCenter(Vec2 newCenter);
	void RotateAboutCenter(float rotationDeltaDegrees);
};