#pragma once
#include "Engine/Math/Vec3.hpp"

struct AABB2;

struct AABB3
{
public:
	Vec3 m_mins;
	Vec3 m_maxs;
	
public:
	static const AABB3 ZERO_TO_ONE;

	~AABB3(){}
	AABB3(){}
	AABB3(AABB3 const& copyForm);
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);

	bool IsPointInside(Vec3 const& point) const;
	Vec3 const GetCenter() const;
	Vec3 const GetDimensions() const;
	Vec3 const GetNearestPoint(Vec3 const& referencePosition) const;
	Vec3 const GetPointAtUV(Vec3 const& uv) const;
	Vec3 const GetUVForPoint(Vec3 const& point) const;

	void Translate(Vec3 const& translationToApply);
	void SetCenter(Vec3 const& newCenter);
	void SetDimensions(Vec3 const& newDimensions);
	void StretchToIncludePoint(Vec3 const& point);

	AABB2 GetAABB2XY() const;
};
