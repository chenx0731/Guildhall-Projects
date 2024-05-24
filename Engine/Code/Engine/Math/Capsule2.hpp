#pragma once
#include "Engine/Math/LineSegment2.hpp"
struct Capsule2
{
	LineSegment2 m_bone;
	float m_radius;

	Capsule2();
	Capsule2(LineSegment2 bone, float radius);

	void Translate(Vec2 translation);
	void SetCenter(Vec2 newCenter);
	void RotateAboutCenter(float rotationDeltaDegrees);
};