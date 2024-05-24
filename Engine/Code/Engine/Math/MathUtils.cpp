#define UNUSED(x) (void)(x);
#pragma once
#include <math.h>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Math/FloatRange.hpp"

const float PI = 3.141592653589793f;

float ConvertDegreesToRadians(float degrees)
{
	return degrees * PI / 180.f;
}
float ConvertRadiansToDegrees(float radians)
{
	return radians * 180.f / PI;
}

float CosDegrees(float degrees)
{
	return cosf(ConvertDegreesToRadians(degrees));
}

float SinDegrees(float degrees)
{
	return sinf(ConvertDegreesToRadians(degrees));
}

float Atan2Degrees(float y, float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

float TanDegrees(float degrees)
{
	return tanf(ConvertDegreesToRadians(degrees));
}

float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	float degree = endDegrees - startDegrees;
	while (degree > 180.f || degree < -180.f)
	{
		if (degree > 180.f)
		{
			degree -= 360.f;
		}
		if (degree < -180.f)
		{
			degree += 360.f;
		}
	}
	return degree;
}

float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	float shortestDegrees = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	if (shortestDegrees < maxDeltaDegrees && shortestDegrees > -maxDeltaDegrees)
		return goalDegrees;
	if (shortestDegrees <= -maxDeltaDegrees)
		return currentDegrees - maxDeltaDegrees;
	else return currentDegrees + maxDeltaDegrees;

}

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
	float abDotProduct = DotProduct2D(a, b);
	float cosAngle = abDotProduct / (a.GetLength() * b.GetLength());
	float degree = acosf(cosAngle) * 180.f / PI;
	return degree;
}


float GetAngleDegreesWithin360(float degree)
{
	while (degree > 360.f || degree < 0.f) {
		if (degree > 360.f)
			degree -= 360.f;
		if (degree < 0.f)
			degree += 360.f;
	}
	return degree;
}

float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return sqrtf((positionA.x - positionB.x) * (positionA.x - positionB.x) 
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y));
}

float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x) 
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrtf((positionA.x - positionB.x) * (positionA.x - positionB.x) 
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y)
		+ (positionA.z - positionB.z) * (positionA.z - positionB.z));
}

float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y)
		+ (positionA.z - positionB.z) * (positionA.z - positionB.z);
}

float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrtf((positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y));
}

float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{
	IntVec2 tempPoint = IntVec2(pointA.x - pointB.x, pointA.y - pointB.y);
	return tempPoint.GetTaxicabLength();
}

float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	float length = vectorToProjectOnto.GetLength();
	if (length == 0.f)
		return 0.f;
	return DotProduct2D(vectorToProject, vectorToProjectOnto) / vectorToProjectOnto.GetLength();
}

Vec2 const GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	return GetProjectedLength2D(vectorToProject, vectorToProjectOnto) * vectorToProjectOnto.GetNormalized();
}

float GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	float length = vectorToProjectOnto.GetLength();
	if (length == 0.f)
		return 0.f;
	return DotProduct3D(vectorToProject, vectorToProjectOnto) / vectorToProjectOnto.GetLength();
}

Vec3 const GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	return GetProjectedLength3D(vectorToProject, vectorToProjectOnto) * vectorToProjectOnto.GetNormalized();
}

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
	float distance = (discCenter - point).GetLengthSquared();
	if (distance < discRadius * discRadius)
	return true;
	return false;
}

bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box)
{
	return box.IsPointInside(point);
}

bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule)
{	
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D(point, capsule.m_bone);
	if ((nearestPoint - point).GetLengthSquared() < capsule.m_radius * capsule.m_radius)
		return true;
	else return false;
}

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Capsule2 capsule = Capsule2(LineSegment2(boneStart, boneEnd), radius);
	return IsPointInsideCapsule2D(point, capsule);
}

bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& orientedBox)
{
	Vec2 pointToLocal = orientedBox.GetLocalPosForWorldPos(point);
	AABB2 aabbBox = AABB2();
	aabbBox.m_mins = -orientedBox.m_halfDimensions;
	aabbBox.m_maxs = orientedBox.m_halfDimensions;
	if (IsPointInsideAABB2D(pointToLocal, aabbBox))
		return true;
	return false;
}

bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius)
{
	Vec2 sectorDirected = Vec2::MakeFromPolarDegrees(sectorForwardDegrees);
	float degree = GetAngleDegreesBetweenVectors2D(point - sectorTip, sectorDirected);
	if (degree > 0.5f * sectorApertureDegrees || degree < -0.5f * sectorApertureDegrees)
	{
		return false;
	}
	float length = (sectorTip - point).GetLength();
	if (length < sectorRadius)
		return true;
	return false;
}

bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius)
{
	float degree = GetAngleDegreesBetweenVectors2D(point - sectorTip, sectorForwardNormal);
	if (degree > 0.5f * sectorApertureDegrees || degree < -0.5f * sectorApertureDegrees)
	{
		return false;
	}
	float length = (sectorTip - point).GetLength();
	if (length < sectorRadius)
		return true;
	return false;
}


bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
	if (GetDistance2D(centerA, centerB) < radiusA + radiusB)
	{
		return true;
	}
	else return false;
}

bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	if (GetDistance3D(centerA, centerB) < radiusA + radiusB)
	{
		return true;
	}
	else return false;
}

Vec2 const GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& disCenter, float discRadius)
{
	float length = (referencePosition - disCenter).GetLengthSquared();
	if (length <= discRadius * discRadius)
	{
		return referencePosition;
	}
	Vec2 normalized = (referencePosition - disCenter).GetNormalized();
	return normalized * discRadius + disCenter;
}

Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& box)
{
	return box.GetNearestPoint(referencePos);
}

Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, LineSegment2 const& infiniteLine)
{
	Vec2 line = infiniteLine.m_end - infiniteLine.m_start;
	Vec2 startToPoint = referencePos - infiniteLine.m_start;
	return infiniteLine.m_start + GetProjectedOnto2D(startToPoint, line);
}

Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine)
{
	LineSegment2 lineSegment = LineSegment2(pointOnLine, anotherPointOnLine);
	return GetNearestPointOnInfiniteLine2D(referencePos, lineSegment);
}

Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, LineSegment2 const& lineSegment)
{
	Vec2 line = lineSegment.m_end - lineSegment.m_start;
	Vec2 startToPoint = referencePos - lineSegment.m_start;
	Vec2 endToPoint = referencePos - lineSegment.m_end;
	if (DotProduct2D(line, startToPoint) < 0.f)
		return lineSegment.m_start;
	if (DotProduct2D(line, endToPoint) > 0.f)
		return lineSegment.m_end;
	return lineSegment.m_start + GetProjectedOnto2D(startToPoint, line);
}

Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 const& lineSegStart, Vec2 const& lineSegEnd)
{
	LineSegment2 lineSegment = LineSegment2(lineSegStart, lineSegEnd);
	return GetNearestPointOnLineSegment2D(referencePos, lineSegment);
}

Vec3 const GetNearestPointOnLineSegment3D(Vec3 const& referencePos, Vec3 const& lineSegStart, Vec3 const& lineSegEnd)
{
	Vec3 line = lineSegEnd - lineSegStart;
	Vec3 startToPoint = referencePos - lineSegStart;
	Vec3 endToPoint = referencePos - lineSegEnd;
	if (DotProduct3D(line, startToPoint) < 0.f)
		return lineSegStart;
	if (DotProduct3D(line, endToPoint) > 0.f)
		return lineSegEnd;
	return lineSegStart + GetProjectedOnto3D(startToPoint, line);
}

Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePos, Capsule2 const& capsule)
{
	Vec2 line = capsule.m_bone.m_end - capsule.m_bone.m_start;
	Vec2 startToPoint = referencePos - capsule.m_bone.m_start;
	Vec2 endToPoint = referencePos - capsule.m_bone.m_end;
	if (DotProduct2D(line, startToPoint) < 0.f)
	{
		return GetNearestPointOnDisc2D(referencePos, capsule.m_bone.m_start, capsule.m_radius);
	}
	if (DotProduct2D(line, endToPoint) > 0.f)
	{
		return GetNearestPointOnDisc2D(referencePos, capsule.m_bone.m_end, capsule.m_radius);
	}
	Vec2 center = 0.5f * line + capsule.m_bone.m_start;
	Vec2 iBasisNormal = line.GetRotatedMinus90Degrees();
	iBasisNormal.Normalize();
	Vec2 halfDimension = Vec2(capsule.m_radius, 0.5f * line.GetLength());
	OBB2 obb2 = OBB2(center, iBasisNormal, halfDimension);
	return GetNearestPointOnOBB2D(referencePos, obb2);
}

Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Capsule2 capsule = Capsule2(LineSegment2(boneStart, boneEnd), radius);
	return GetNearestPointOnCapsule2D(referencePos, capsule);
}

Vec2 const GetNearestPointOnOBB2D(Vec2 const& referencePos, OBB2 const& orientedBox)
{
	Vec2 centerToPos = referencePos - orientedBox.m_center;
	Vec2 jBasis = orientedBox.m_iBasisNormal.GetRotated90Degrees();
	float pTC_iBasis = DotProduct2D(centerToPos, orientedBox.m_iBasisNormal);
	float pTC_jBasis = DotProduct2D(centerToPos, jBasis);
	pTC_iBasis = GetClamped(pTC_iBasis, -orientedBox.m_halfDimensions.x, orientedBox.m_halfDimensions.x);
	pTC_jBasis = GetClamped(pTC_jBasis, -orientedBox.m_halfDimensions.y, orientedBox.m_halfDimensions.y);
	return orientedBox.m_center + pTC_iBasis * orientedBox.m_iBasisNormal + pTC_jBasis * jBasis;
}

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint)
{
	if ((mobileDiscCenter - fixedPoint).GetLengthSquared() >= discRadius * discRadius)
		return false;
	Vec2 normalized = (mobileDiscCenter - fixedPoint).GetNormalized();
	mobileDiscCenter += normalized * (discRadius - (mobileDiscCenter - fixedPoint).GetLength());
	return true;
}

bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	if ((mobileDiscCenter - fixedDiscCenter).GetLengthSquared() >= (mobileDiscRadius + fixedDiscRadius) * (mobileDiscRadius + fixedDiscRadius))
		return false;
	Vec2 normalized = (mobileDiscCenter - fixedDiscCenter).GetNormalized();
	float distance = (mobileDiscCenter - fixedDiscCenter).GetLength();
	mobileDiscCenter += normalized * (mobileDiscRadius + fixedDiscRadius - distance);
	return true;
}

bool PushDiscOutOfFixedDisc3D(Vec3& mobileDiscCenter, float mobileDiscRadius, Vec3 const& fixedDiscCenter, float fixedDiscRadius)
{
	if ((mobileDiscCenter - fixedDiscCenter).GetLengthSquared() >= (mobileDiscRadius + fixedDiscRadius) * (mobileDiscRadius + fixedDiscRadius))
		return false;
	Vec3 normalized = (mobileDiscCenter - fixedDiscCenter).GetNormalized();
	float distance = (mobileDiscCenter - fixedDiscCenter).GetLength();
	mobileDiscCenter += normalized * (mobileDiscRadius + fixedDiscRadius - distance);
	return true;
}

bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius)
{
	if ((aCenter - bCenter).GetLengthSquared() >= (aRadius + bRadius) * (aRadius + bRadius))
		return false;
	Vec2 normalizedAtoB = (bCenter - aCenter).GetNormalized();
	float distance = (bCenter - aCenter).GetLength();
	bCenter += normalizedAtoB * 0.5f * (aRadius + bRadius - distance);
	aCenter -= normalizedAtoB * 0.5f * (aRadius + bRadius - distance);
	return true;
}

bool PushDiscsOutOfEachOther3D(Vec3& aCenter, float aRadius, Vec3& bCenter, float bRadius)
{
	if ((aCenter - bCenter).GetLengthSquared() >= (aRadius + bRadius) * (aRadius + bRadius))
		return false;
	Vec3 normalizedAtoB = (bCenter - aCenter).GetNormalized();
	float distance = (bCenter - aCenter).GetLength();
	bCenter += normalizedAtoB * 0.5f * (aRadius + bRadius - distance);
	aCenter -= normalizedAtoB * 0.5f * (aRadius + bRadius - distance);
	return true;
}

bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
	Vec2 nearestPoint = fixedBox.GetNearestPoint(mobileDiscCenter);
	if ((mobileDiscCenter - nearestPoint).GetLengthSquared() >= discRadius * discRadius)
		return false;
	PushDiscOutOfFixedPoint2D(mobileDiscCenter, discRadius, nearestPoint);
	return true;
}

bool PushAABB2DOutOfFixedPoint2D(AABB2& mobileAABB, Vec2 const& fixedPoint)
{
	if (!mobileAABB.IsPointInside(fixedPoint))
		return false;
	float distPtoMinX = fixedPoint.x - mobileAABB.m_mins.x;
	float distPtoMinY = fixedPoint.y - mobileAABB.m_mins.y;
	float distPtoMaxX = mobileAABB.m_maxs.x - fixedPoint.x;
	float distPtoMaxY = mobileAABB.m_maxs.y - fixedPoint.y;

	if (distPtoMinX <= MinNumAmong3(distPtoMinY, distPtoMaxX, distPtoMaxY)) {
		Vec2 nowCenter = mobileAABB.GetCenter();
		nowCenter.x += distPtoMinX;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMinY <= MinNumAmong3(distPtoMinX, distPtoMaxX, distPtoMaxY)) {
		Vec2 nowCenter = mobileAABB.GetCenter();
		nowCenter.y += distPtoMinY;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMaxX <= MinNumAmong3(distPtoMinY, distPtoMinX, distPtoMaxY)) {
		Vec2 nowCenter = mobileAABB.GetCenter();
		nowCenter.x -= distPtoMaxX;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMaxY <= MinNumAmong3(distPtoMinY, distPtoMaxX, distPtoMinX)) {
		Vec2 nowCenter = mobileAABB.GetCenter();
		nowCenter.y -= distPtoMaxY;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	return false;
}

bool PushAABB2DOutOfFixedAABB2D(AABB2& mobileAABB, AABB2 const& fixedAABB)
{
	Vec2 mobileCenter = mobileAABB.GetCenter();
	Vec2 nearestPoint = fixedAABB.GetNearestPoint(mobileCenter);
	return PushAABB2DOutOfFixedPoint2D(mobileAABB, nearestPoint);
}

bool PushCylinder3DOutOfAABB3D(Vec3& cylinderBottomCenter, float cylinderRadius, float cylinderHeight, AABB3& aabb3, bool isCldMovable, bool isAABBMovable)
{
	if (!isAABBMovable && !isCldMovable)
		return false;
	Vec2 centerA = aabb3.GetCenter().GetVec2XY();
	Vec2 centerB = cylinderBottomCenter.GetVec2XY();
	bool isDiscOverlap = false;
	bool isZOverlap = false;
	AABB2 aabb2A = aabb3.GetAABB2XY();

	Vec2 nearestToAABB = aabb2A.GetNearestPoint(centerB);
	if (GetDistanceSquared2D(nearestToAABB, centerB) < cylinderRadius * cylinderRadius) {
		isDiscOverlap = true;
	}
	FloatRange a = FloatRange(aabb3.m_mins.z, aabb3.m_maxs.z);
	FloatRange b = FloatRange(cylinderBottomCenter.z, cylinderBottomCenter.z + cylinderHeight);
	if (a.IsOverlappingWith(b)) {
		isZOverlap = true;
	}
	if (!isDiscOverlap || !isZOverlap)
		return false;

	if (isDiscOverlap) {
		if (!isAABBMovable) {
			float tolerance = 0.13f;
				//0.13f;
			//if (a.m_max - cylinderBottomCenter.z < cylinderBottomCenter.z  - a.m_min && )
			if (b.m_max < a.m_min + tolerance && b.m_max > a.m_min) {
				cylinderBottomCenter.z = a.m_min - cylinderHeight;
			}
			else if (b.m_min > a.m_max - tolerance && b.m_min < a.m_max) {
				cylinderBottomCenter.z = a.m_max;
			}
			else PushDiscOutOfFixedAABB2D(centerB, cylinderRadius, aabb2A);
			//centerA = tempCenterA;

			//PushDiscOutOfFixedDisc2D(centerB, actorB->m_actorDef->m_radius, centerA, actorA->m_actorDef->m_radius);
		}

		else if (!isCldMovable) {
			Vec2 tempCenterB = centerB;
			PushDiscOutOfFixedAABB2D(tempCenterB, cylinderRadius, aabb2A);
			centerA -= (tempCenterB - centerB);
			//PushDiscOutOfFixedAABB2D(centerA, actorA->m_actorDef->m_radius, aabb2A);
		}

		else {
			Vec2 tempCenterB = centerB;
			PushDiscOutOfFixedAABB2D(tempCenterB, cylinderRadius, aabb2A);
			Vec2 deltaDirection = (tempCenterB - centerB) * 0.5f;
			centerA -= deltaDirection;
			centerB += deltaDirection;
		}
		cylinderBottomCenter.x = centerB.x;
		cylinderBottomCenter.y = centerB.y;

		Vec3 newAABBCenter = Vec3(centerA.x, centerB.x, aabb3.GetCenter().z);
		aabb3.SetCenter(newAABBCenter);
		return true;
	}
	return false;
}

bool PushAABB3DOutOfFixedPoint3D(AABB3& mobileAABB, Vec3 const& fixedPoint)
{
	if (!mobileAABB.IsPointInside(fixedPoint))
		return false;
	float distPtoMinX = fixedPoint.x - mobileAABB.m_mins.x;
	float distPtoMinY = fixedPoint.y - mobileAABB.m_mins.y;
	float distPtoMinZ = fixedPoint.z - mobileAABB.m_mins.z;
	float distPtoMaxX = mobileAABB.m_maxs.x - fixedPoint.x;
	float distPtoMaxY = mobileAABB.m_maxs.y - fixedPoint.y;
	float distPtoMaxZ = mobileAABB.m_maxs.z - fixedPoint.z;

	if (distPtoMinX <= MinNumAmong3(distPtoMinY, distPtoMinZ, MinNumAmong3(distPtoMaxX, distPtoMaxY, distPtoMaxZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.x += distPtoMinX;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMinY <= MinNumAmong3(distPtoMinX, distPtoMinZ, MinNumAmong3(distPtoMaxX, distPtoMaxY, distPtoMaxZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.y += distPtoMinY;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMinZ <= MinNumAmong3(distPtoMinX, distPtoMinY, MinNumAmong3(distPtoMaxX, distPtoMaxY, distPtoMaxZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.z += distPtoMinZ;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMaxX <= MinNumAmong3(distPtoMaxY, distPtoMaxZ, MinNumAmong3(distPtoMinX, distPtoMinY, distPtoMinZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.x -= distPtoMaxX;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMaxY <= MinNumAmong3(distPtoMaxZ, distPtoMaxX, MinNumAmong3(distPtoMinX, distPtoMinY, distPtoMinZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.y -= distPtoMaxY;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	if (distPtoMaxZ <= MinNumAmong3(distPtoMaxY, distPtoMaxX, MinNumAmong3(distPtoMinX, distPtoMinY, distPtoMinZ))) {
		Vec3 nowCenter = mobileAABB.GetCenter();
		nowCenter.z -= distPtoMaxZ;
		mobileAABB.SetCenter(nowCenter);
		return true;
	}
	return false;
}

bool PushAABB3DOutOfFixedAABB3D(AABB3& mobileAABB, AABB3 const& fixedAABB)
{
	Vec3 mobileCenter = mobileAABB.GetCenter();
	Vec3 nearestPoint = fixedAABB.GetNearestPoint(mobileCenter);
	return PushAABB3DOutOfFixedPoint3D(mobileAABB, nearestPoint);
}

bool BounceDiscOffFixedPoint2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const Vec2& point, float mobileElasticity, float fixedElasticity)
{
	if ((mobileDiscCenter - point).GetLengthSquared() >= (mobileDiscRadius) * (mobileDiscRadius))
		return false;
	Vec2 normalized = (point - mobileDiscCenter).GetNormalized();
	PushDiscOutOfFixedPoint2D(mobileDiscCenter, mobileDiscRadius, point);
	Vec2 velOnNormal = GetProjectedOnto2D(mobileDiscVel, normalized);
	Vec2 velOnT = mobileDiscVel - velOnNormal;
	if ((mobileDiscCenter - velOnNormal - point).GetLengthSquared() > (mobileDiscCenter + velOnNormal - point).GetLengthSquared())
		mobileDiscVel = velOnT - velOnNormal * mobileElasticity * fixedElasticity;
	else
		mobileDiscVel = velOnT + velOnNormal * mobileElasticity * fixedElasticity;
	return true;
}

bool BounceDiscOffEachOther2D(Vec2& aCenter, float aRadius, Vec2& aVel, Vec2& bCenter, float bRadius, Vec2& bVel, float aElasticity, float bElasticity)
{
	if ((aCenter - bCenter).GetLengthSquared() >= (aRadius + bRadius) * (aRadius + bRadius))
		return false;
	Vec2 normalAtoB = (bCenter - aCenter).GetNormalized();
	PushDiscsOutOfEachOther2D(aCenter, aRadius, bCenter, bRadius);
	Vec2 aVelOnNormal = GetProjectedOnto2D(aVel, normalAtoB);
	Vec2 aVelOnT = aVel - aVelOnNormal;
	Vec2 bVelOnNormal = GetProjectedOnto2D(bVel, normalAtoB);
	Vec2 bVelOnT = bVel - bVelOnNormal;

	float nonExchange  = (aVelOnNormal + aCenter - bVelOnNormal - bCenter).GetLengthSquared();
	float exchange = (bVelOnNormal + aCenter - aVelOnNormal - bCenter).GetLengthSquared();
	if (exchange > nonExchange) {
		aVel = bVelOnNormal * aElasticity * bElasticity + aVelOnT;
		bVel = aVelOnNormal * aElasticity * bElasticity + bVelOnT;
	}
	else {
		aVel = aVelOnNormal * aElasticity * bElasticity + aVelOnT;
		bVel = bVelOnNormal * aElasticity * bElasticity + bVelOnT;
	}
	
	return true;
}

bool BounceDiscOffEachOther3D(Vec3& aCenter, float aRadius, Vec3& aVel, Vec3& bCenter, float bRadius, Vec3& bVel, float aElasticity, float bElasticity)
{
	if ((aCenter - bCenter).GetLengthSquared() >= (aRadius + bRadius) * (aRadius + bRadius))
		return false;
	Vec3 normalAtoB = (bCenter - aCenter).GetNormalized();
	PushDiscsOutOfEachOther3D(aCenter, aRadius, bCenter, bRadius);
	Vec3 aVelOnNormal = GetProjectedOnto3D(aVel, normalAtoB);
	Vec3 aVelOnT = aVel - aVelOnNormal;
	Vec3 bVelOnNormal = GetProjectedOnto3D(bVel, normalAtoB);
	Vec3 bVelOnT = bVel - bVelOnNormal;

	float nonExchange = (aVelOnNormal + aCenter - bVelOnNormal - bCenter).GetLengthSquared();
	float exchange = (bVelOnNormal + aCenter - aVelOnNormal - bCenter).GetLengthSquared();
	if (exchange > nonExchange) {
		aVel = bVelOnNormal * aElasticity * bElasticity + aVelOnT;
		bVel = aVelOnNormal * aElasticity * bElasticity + bVelOnT;
	}
	else {
		aVel = aVelOnNormal * aElasticity * bElasticity + aVelOnT;
		bVel = bVelOnNormal * aElasticity * bElasticity + bVelOnT;
	}

	return true;
}

bool BounceDiscOffFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, Vec2& fixedDiscCenter, float fixedDiscRadius, float mobileElasticity, float fixedElasticity)
{
	if ((mobileDiscCenter - fixedDiscCenter).GetLengthSquared() >= (mobileDiscRadius + fixedDiscRadius) * (mobileDiscRadius + fixedDiscRadius))
		return false;
	Vec2 normalized = (fixedDiscCenter - mobileDiscCenter).GetNormalized();
	PushDiscOutOfFixedDisc2D(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius);
	Vec2 velOnNormal = GetProjectedOnto2D(mobileDiscVel, normalized);
	Vec2 velOnT = mobileDiscVel - velOnNormal;
	mobileDiscVel = velOnT - velOnNormal * mobileElasticity * fixedElasticity;
	return true;
}

bool BounceDiscOffFixedDisc3D(Vec3& mobileDiscCenter, float mobileDiscRadius, Vec3& mobileDiscVel, Vec3& fixedDiscCenter, float fixedDiscRadius, float mobileElasticity, float fixedElasticity)
{
	if ((mobileDiscCenter - fixedDiscCenter).GetLengthSquared() >= (mobileDiscRadius + fixedDiscRadius) * (mobileDiscRadius + fixedDiscRadius))
		return false;
	Vec3 normalized = (fixedDiscCenter - mobileDiscCenter).GetNormalized();
	PushDiscOutOfFixedDisc3D(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius);
	Vec3 velOnNormal = GetProjectedOnto3D(mobileDiscVel, normalized);
	Vec3 velOnT = mobileDiscVel - velOnNormal;
	mobileDiscVel = velOnT - velOnNormal * mobileElasticity * fixedElasticity;
	return true;
}

bool BounceDiscOffFixedOBB2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const OBB2& fixedOBB, float mobileElasticity, float fixedElasticity)
{
	Vec2 nearestPoint = GetNearestPointOnOBB2D(mobileDiscCenter, fixedOBB);
	return BounceDiscOffFixedPoint2D(mobileDiscCenter, mobileDiscRadius, mobileDiscVel, nearestPoint, mobileElasticity, fixedElasticity);
	//if ((nearestPoint - mobileDiscCenter).GetLengthSquared() > mobileDiscRadius * mobileDiscRadius)
	//	return false;
}

bool BounceDiscOffFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const Capsule2& fixedCapsule, float mobileElasticity, float fixedElasticity)
{
	Vec2 nearestPoint = GetNearestPointOnCapsule2D(mobileDiscCenter, fixedCapsule);
	return BounceDiscOffFixedPoint2D(mobileDiscCenter, mobileDiscRadius, mobileDiscVel, nearestPoint, mobileElasticity, fixedElasticity);
}

void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
	posToTransform = posToTransform * uniformScale;
	posToTransform = posToTransform.GetRotatedDegrees(rotationDegrees);
	posToTransform.x += translation.x;
	posToTransform.y += translation.y;
}

void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	posToTransform = posToTransform.x * iBasis + posToTransform.y * jBasis + translation;
}

void TransformPositionXY3D(Vec3& posToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY)
{
	posToTransform.x *= scaleXY;
	posToTransform.y *= scaleXY;
	posToTransform = posToTransform.GetRotatedAboutZDegrees(zRotationDegrees);
	posToTransform.x += translationXY.x;
	posToTransform.y += translationXY.y;
}

void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	Vec2 temp = Vec2(posToTransform.x, posToTransform.y);
	TransformPosition2D(temp, iBasis, jBasis, translation);
	posToTransform.x = temp.x;
	posToTransform.y = temp.y;
}

void TransformPositionXYZ3D(Vec3& posToTransform, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation)
{
	posToTransform = posToTransform.x * iBasis + posToTransform.y * jBasis + posToTransform.z * kBasis + translation;
}

float GetClamped(float value, float minValue, float maxValue)
{
	if (value < minValue)
		return minValue;
	if (value > maxValue)
		return maxValue;
	return value;
}

int	GetClamped(int value, int minValue, int maxValue)
{
	if (value < minValue)
		return minValue;
	if (value > maxValue)
		return maxValue;
	return value;
}

float GetClampedZeroToOne(float value)
{
	return GetClamped(value, 0.f, 1.f);
}

float Interpolate(float start, float end, float fractionTowardEnd)
{
	return start + (end - start) * fractionTowardEnd;
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	return Interpolate(outStart, outEnd, GetFractionWithinRange(inValue, inStart, inEnd));
}

float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	inValue = GetClamped(inValue, inStart, inEnd);
	return Interpolate(outStart, outEnd, GetFractionWithinRange(inValue, inStart, inEnd));
}

int RoundDownToInt(float value)
{
	return static_cast<int> (floor(value));
}

int RoundDownToInt(double value)
{
	return static_cast<int> (floor(value));
}

float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.x + a.y * b.y;
}

float DotProduct3D(Vec3 const& a, Vec3 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DotProduct4D(Vec4 const& a, Vec4 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.y - a.y * b.x;
}

Vec3  CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float NormalizeByte(unsigned char byteValue)
{
	return float(byteValue) / 255.f;
}

unsigned char DenormalizeByte(float zeroToOne)
{
	float scaled = zeroToOne * 256.f;
	int byte = RoundDownToInt(scaled);
	if (zeroToOne == 1.f)
		return 255;
	else return (unsigned char)byte;
}

RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius)
{
	RaycastResult2D result2D;

	Vec2 impactPos = Vec2(0.f, 0.f);
	bool didImpact = false;
	float impactDis = 0.f;
	Vec2 impactNormal = Vec2(0.f, 0.f);
	

	Vec2 startToCenter = discCenter - startPos;
	Vec2 jFwdNormal = fwdNormal.GetRotated90Degrees();
	float sTCenterOnJ = DotProduct2D(startToCenter, jFwdNormal);
	float sTCenterOnI = DotProduct2D(startToCenter, fwdNormal);
	if (sTCenterOnJ * sTCenterOnJ >= discRadius * discRadius) {
		//return RaycastResult2D(impactPos, didImpact, impactDis, impactNormal);
		return result2D;
	}
	if (sTCenterOnI <= -discRadius || sTCenterOnI >= maxDist + discRadius) {
		//return RaycastResult2D(impactPos, didImpact, impactDis, impactNormal);
		return result2D;
	}
	if (IsPointInsideDisc2D(startPos, discCenter, discRadius)) {
		return RaycastResult2D(startPos, true, impactDis, -startToCenter.GetNormalized());
	}
	float subtract = sqrtf(discRadius * discRadius - sTCenterOnJ * sTCenterOnJ);
	float distance = sTCenterOnI - subtract;
	if (distance >= maxDist || distance <= 0.f) {
		//return RaycastResult2D(impactPos, didImpact, impactDis, impactNormal);
		return result2D;
	}
	impactPos = startPos + fwdNormal * distance;
	didImpact = true;
	impactDis = distance;
	impactNormal = (impactPos - discCenter).GetNormalized();

	result2D.m_impactPos = startPos + fwdNormal * distance;
	result2D.m_didImpact = true;
	result2D.m_impactDis = distance;
	result2D.m_impactNormal = (impactPos - discCenter).GetNormalized();
	result2D.m_exitDis = sTCenterOnI + subtract;
	//return RaycastResult2D(impactPos, didImpact, impactDis, impactNormal);
	return result2D;
}

RaycastResult2D RaycastVsLine2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 line)
{
	RaycastResult2D result;
	Vec2 jNormal = fwdNormal.GetRotated90Degrees();
	Vec2 SStart = line.m_start - startPos;
	Vec2 SEnd = line.m_end - startPos;
	float sStartOnJ = DotProduct2D(SStart, jNormal);
	float sEndOnJ = DotProduct2D(SEnd, jNormal);
	// straddle test
	if (sStartOnJ * sEndOnJ >= 0.f)
		return result;

	float t = -sStartOnJ / (sEndOnJ - sStartOnJ);
	Vec2 impactPos = line.m_start + t * (line.m_end - line.m_start);
	float impactDist = DotProduct2D((impactPos - startPos), fwdNormal);
	// too early or too late
	if (impactDist <= 0 || impactDist >= maxDist)
		return result;

	Vec2 lineFwd = line.m_end - line.m_start;
	lineFwd.Normalize();
	Vec2 impactNormal = lineFwd.GetRotated90Degrees();
	if (DotProduct2D(impactNormal, fwdNormal) > 0.f)
		impactNormal = -impactNormal;
	result.m_didImpact = true;
	result.m_impactDis = impactDist;
	result.m_impactPos = impactPos;
	result.m_impactNormal = impactNormal;
	return result;
}

RaycastResult2D RaycastVsInfiniteLine2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 line)
{
	UNUSED(maxDist);
	RaycastResult2D result;
	Vec2 jNormal = fwdNormal.GetRotated90Degrees();
	Vec2 SStart = line.m_start - startPos;
	Vec2 SEnd = line.m_end - startPos;
	float sStartOnJ = DotProduct2D(SStart, jNormal);
	float sEndOnJ = DotProduct2D(SEnd, jNormal);
	// straddle test
	if (sStartOnJ * sEndOnJ == 0.f)
		return result;

	float t = -sStartOnJ / (sEndOnJ - sStartOnJ);
	Vec2 impactPos = line.m_start + t * (line.m_end - line.m_start);
	float impactDist = DotProduct2D((impactPos - startPos), fwdNormal);
	// too early or too late
	//if (impactDist <= 0 || impactDist >= maxDist)
		//return result;

	Vec2 lineFwd = line.m_end - line.m_start;
	lineFwd.Normalize();
	Vec2 impactNormal = lineFwd.GetRotated90Degrees();
	if (DotProduct2D(impactNormal, fwdNormal) > 0.f)
		impactNormal = -impactNormal;
	result.m_didImpact = true;
	result.m_impactDis = impactDist;
	result.m_impactPos = impactPos;
	result.m_impactNormal = impactNormal;
	return result;
}

RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 aabb)
{
	RaycastResult2D result;
	
	if (IsPointInsideAABB2D(startPos, aabb)) {
		result.m_didImpact = true;
		result.m_impactPos = startPos;
		result.m_impactNormal = -fwdNormal;
		result.m_exitDis = 0.f;
		result.m_impactDis = 0.f;
		return result;
	}

	Vec2 leftBottom = aabb.m_mins;
	Vec2 leftTop = Vec2(aabb.m_mins.x, aabb.m_maxs.y);
	Vec2 rightBottom = Vec2(aabb.m_maxs.x, aabb.m_mins.y);
	Vec2 rightTop = aabb.m_maxs;

	LineSegment2 bottom = LineSegment2(leftBottom, rightBottom);
	LineSegment2 top = LineSegment2(leftTop, rightTop);
	LineSegment2 left = LineSegment2(leftBottom, leftTop);
	LineSegment2 right = LineSegment2(rightBottom, rightTop);

	RaycastResult2D resultX1 = RaycastVsInfiniteLine2D(startPos, fwdNormal, maxDist, bottom);
	RaycastResult2D resultX2 = RaycastVsInfiniteLine2D(startPos, fwdNormal, maxDist, top);
	RaycastResult2D resultY1 = RaycastVsInfiniteLine2D(startPos, fwdNormal, maxDist, left);
	RaycastResult2D resultY2 = RaycastVsInfiniteLine2D(startPos, fwdNormal, maxDist, right);

	if (!resultX1.m_didImpact || !resultX2.m_didImpact || !resultY1.m_didImpact || !resultY2.m_didImpact)
		return result;

	float tX1 = resultX1.m_impactDis;
	float tX2 = resultX2.m_impactDis;
	float tY1 = resultY1.m_impactDis;
	float tY2 = resultY2.m_impactDis;

	if (tX1 > tX2) {
		float temp = tX1;
		tX1 = tX2;
		tX2 = temp;

		RaycastResult2D tempResult = resultX1;
		resultX1 = resultX2;
		resultX2 = tempResult;
	}

	if (tY1 > tY2) {
		float temp = tY1;
		tY1 = tY2;
		tY2 = temp;

		RaycastResult2D tempResult = resultY1;
		resultY1 = resultY2;
		resultY2 = tempResult;
	}
	FloatRange timeForX = FloatRange(tX1, tX2);
	FloatRange timeForY = FloatRange(tY1, tY2);

	if (!timeForX.IsOverlappingWith(timeForY))
		return result;

	Vec2 impactPos;
	if (tX1 > tY1) {
		impactPos = resultX1.m_impactPos;
		if (IsPointInsideAABB2D(impactPos, aabb) && resultX1.m_impactDis >= 0.f) {
			if (tX2 < tY2)
				resultX1.m_exitDis = resultX2.m_impactDis;
			else resultX1.m_exitDis = resultY2.m_impactDis;
			return resultX1;
		}
	}
	else
	{
		impactPos = resultY1.m_impactPos;
		if (IsPointInsideAABB2D(impactPos, aabb) && resultY1.m_impactDis >= 0.f) {
			if (tX2 < tY2)
				resultY1.m_exitDis = resultX2.m_impactDis;
			else resultY1.m_exitDis = resultY2.m_impactDis;
			return resultY1;
		}
	}
	return result;
}

RaycastResult3D RaycastVsCylinderZ3D(const Vec3& start, const Vec3& direction, float distance, const Vec2& center, float minZ, float maxZ, float radius)
{
	RaycastResult3D result3D;

	float speed = direction.GetLength();
	float scaleXY = direction.GetVec2XY().GetLength();
	float scaleZ = fabsf(direction.z);
	float endZ = start.z + distance * direction.z;
	FloatRange zRange;
	FloatRange zCylinderRange = FloatRange(minZ, maxZ);
	if (start.z > endZ)
		zRange = FloatRange(endZ, start.z);
	else
		zRange = FloatRange(start.z, endZ);

	bool didZImpact = zRange.IsOverlappingWith(zCylinderRange) || zCylinderRange.IsOverlappingWith(zRange);
	RaycastResult2D result2D = RaycastVsDisc2D(start.GetVec2XY(), direction.GetVec2XY().GetNormalized(), distance * scaleXY, center, radius);
	// didn't impact
	if (!result2D.m_didImpact || !didZImpact) {
		return result3D;
	}

	float tDiscIn = result2D.m_impactDis / (scaleXY * speed);
	float tDiscOut = result2D.m_exitDis / (scaleXY * speed);
	if (tDiscIn == tDiscOut)
		tDiscOut = 999999999.f;
	FloatRange tDiscXY = FloatRange(tDiscIn, tDiscOut);

	if (direction.z == 0.f) {
		result3D.m_didImpact = true;
		result3D.m_impactDis = result2D.m_impactDis;
		result3D.m_impactPos = result2D.m_impactPos;
		result3D.m_impactPos.z = start.z;
		result3D.m_impactNormal = result2D.m_impactNormal;
		return result3D;
	}

	float tZIn = fabsf(GetClamped(start.z, minZ, maxZ) - start.z) / (scaleZ * speed);
	float tZOut = 0.f;
	if (direction.z > 0.f) {
		if (start.z < maxZ)
			tZOut = (maxZ - start.z) / (scaleZ * speed);
	}
	else {
		if (start.z > minZ)
			tZOut = (start.z - minZ) / (scaleZ * speed);
	}
	FloatRange tZ = FloatRange::ZERO;
	if (tZOut > tZIn)
	tZ = FloatRange(tZIn, tZOut);

	if (tDiscXY.IsOverlappingWith(tZ) || tZ.IsOverlappingWith(tDiscXY)) {
		result3D.m_didImpact = true;
		if (tZ.IsOnRange(tDiscIn)) {
		//if (tDiscIn < tZIn)	{
			result3D.m_impactPos = start + tDiscIn * direction ;
			result3D.m_impactDis = tDiscIn;
			result3D.m_impactNormal = result2D.m_impactNormal;
		}
		else {
			result3D.m_impactPos = start + tZIn * direction;
			result3D.m_impactDis = tZIn;
			if (direction.z > 0.f)
				result3D.m_impactNormal = Vec3(0.f, 0.f, -1.f);
			else
				result3D.m_impactNormal = Vec3(0.f, 0.f, 1.f);
		}
	}
	return result3D;
}

RaycastResult3D RaycastVsAABB3D(const Vec3& start, const Vec3& direction, float distance, AABB3 aabb)
{
	RaycastResult3D result3D;

	float speed = direction.GetLength();
	float scaleXY = direction.GetVec2XY().GetLength();
	float scaleZ = fabsf(direction.z);
	float endZ = start.z + distance * direction.z;
	FloatRange zRange;
	FloatRange zAABB3Range = FloatRange(aabb.m_mins.z, aabb.m_maxs.z);
	if (start.z > endZ)
		zRange = FloatRange(endZ, start.z);
	else
		zRange = FloatRange(start.z, endZ);

	bool didZImpact = zRange.IsOverlappingWith(zAABB3Range) || zAABB3Range.IsOverlappingWith(zRange);
	RaycastResult2D result2D = RaycastVsAABB2D(start.GetVec2XY(), direction.GetVec2XY(), distance * scaleXY, AABB2(aabb.m_mins.GetVec2XY(), aabb.m_maxs.GetVec2XY()));
		//RaycastVsDisc2D(start.GetVec2XY(), direction.GetVec2XY().GetNormalized(), distance * scaleXY, center, radius);
	// didn't impact
	if (!result2D.m_didImpact || !didZImpact) {
		return result3D;
	}
	float tDiscIn;
	float tDiscOut;
	if (scaleXY == 0.f) {
		tDiscIn = 0.f;
		tDiscOut = 0.f;
	}
	else {
		tDiscIn = result2D.m_impactDis / (scaleXY * speed);
		tDiscOut = result2D.m_exitDis / (scaleXY * speed);
	}
	if (tDiscIn == tDiscOut)
		tDiscOut = 999999999.f;
	FloatRange tDiscXY = FloatRange(tDiscIn, tDiscOut);

	if (direction.z == 0.f) {
		result3D.m_didImpact = true;
		result3D.m_impactDis = result2D.m_impactDis;
		result3D.m_impactPos = result2D.m_impactPos;
		result3D.m_impactPos.z = start.z;
		result3D.m_impactNormal = result2D.m_impactNormal;
		return result3D;
	}

	float tZIn = fabsf(GetClamped(start.z, aabb.m_mins.z, aabb.m_maxs.z) - start.z) / (scaleZ * speed);
	float tZOut = 0.f;
	if (direction.z > 0.f) {
		if (start.z < aabb.m_maxs.z)
			tZOut = (aabb.m_maxs.z - start.z) / (scaleZ * speed);
	}
	else {
		if (start.z > aabb.m_mins.z)
			tZOut = (start.z - aabb.m_mins.z) / (scaleZ * speed);
	}
	FloatRange tZ = FloatRange::ZERO;
	if (tZOut > tZIn)
		tZ = FloatRange(tZIn, tZOut);

	if (tDiscXY.IsOverlappingWith(tZ) || tZ.IsOverlappingWith(tDiscXY)) {
		result3D.m_didImpact = true;
		if (tZ.IsOnRange(tDiscIn)) {
			//if (tDiscIn < tZIn)	{
			result3D.m_impactPos = start + tDiscIn * direction;
			result3D.m_impactDis = tDiscIn;
			result3D.m_impactNormal = result2D.m_impactNormal;
		}
		else {
			result3D.m_impactPos = start + tZIn * direction;
			result3D.m_impactDis = tZIn;
			if (direction.z > 0.f)
				result3D.m_impactNormal = Vec3(0.f, 0.f, -1.f);
			else
				result3D.m_impactNormal = Vec3(0.f, 0.f, 1.f);
		}
	}
	return result3D;
}

void SwapFloatValue(float& a, float& b)
{
	float temp = a;
	a = b;
	b = temp;
}

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& cameraMatrix, const Vec3& billboardPosition, const Vec2& billboardScale)
{
	Mat44 res;
	Vec3 i = Vec3(1.f, 0.f, 0.f);
	Vec3 j = Vec3(0.f, 1.f, 0.f);
	Vec3 k = Vec3(0.f, 0.f, 1.f);
	Vec3 t = billboardPosition;

	Vec3 forward = cameraMatrix.GetIBasis3D();
	Vec3 left = cameraMatrix.GetJBasis3D();
	Vec3 up = cameraMatrix.GetKBasis3D();
	Vec3 pos = cameraMatrix.GetTranslation3D();

	Vec3 z = Vec3(0.f, 0.f, 1.f);
	Vec3 y = Vec3(0.f, 1.f, 0.f);

	switch (billboardType)
	{
	case BillboardType::FULL_CAMERA_FACING: 
		i = pos - t;
		if (i.GetLengthSquared() != 0)
		i.Normalize();
		else {
			i = -forward;
			j = -left;
			k = up;
		}
		if (fabs(i.z) < 1) {
			j = CrossProduct3D(z, i);
			j.Normalize();
			k = CrossProduct3D(i, j);
			k.Normalize();
		}
		else {
			k = CrossProduct3D(i, y);
			k.Normalize();
			j = CrossProduct3D(k, i);
			j.Normalize();
		}
		break;
	case BillboardType::FULL_CAMERA_OPPOSING:
		i = -forward;
		j = -left;
		k = up;
		break;
	case BillboardType::WORLD_UP_CAMERA_FACING:
		k = z;
		i = pos - t;
		i.z = 0.f;
		i.Normalize();
		j = CrossProduct3D(k, i);
		j.Normalize();
		break;
	case BillboardType::WORLD_UP_CAMERA_OPPOSING:
		k = z;
		i = -forward;
		i.z = 0.f;
		i.Normalize();
		j = CrossProduct3D(k, i);
		j.Normalize();
		break;
	}
	res.SetIJKT3D(i, j, k, t);
	res.AppendScaleNonUniform2D(billboardScale);
	return res;
}

float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	float AB = Interpolate(A, B, t);
	float BC = Interpolate(B, C, t);
	float CD = Interpolate(C, D, t);

	float ABC = Interpolate(AB, BC, t);
	float BCD = Interpolate(BC, CD, t);

	return Interpolate(ABC, BCD, t);
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	float ABCD = ComputeCubicBezier1D(A, B, C, D, t);
	float BCDE = ComputeCubicBezier1D(B, C, D, E, t);
	float CDEF = ComputeCubicBezier1D(C, D, E, F, t);

	float ABCDE = Interpolate(ABCD, BCDE, t);
	float BCDEF = Interpolate(BCDE, CDEF, t);

	return Interpolate(ABCDE, BCDEF, t);
}

float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	return SmoothStart2(t) * t;
}

float SmoothStart4(float t)
{
	return SmoothStart3(t) * t;
}

float SmoothStart5(float t)
{
	return SmoothStart4(t) * t;
}

float SmoothStart6(float t)
{
	return SmoothStart5(t) * t;
}

float SmoothStop2(float t)
{
	t = 1.f - t;
	return 1.f - t * t;
}

float SmoothStop3(float t)
{
	t = 1.f - t;
	return 1.f - SmoothStart3(t);
}

float SmoothStop4(float t)
{
	t = 1.f - t;
	return 1.f - SmoothStart4(t);
}

float SmoothStop5(float t)
{
	t = 1.f - t;
	return 1.f - SmoothStart5(t);
}

float SmoothStop6(float t)
{
	t = 1.f - t;
	return 1.f - SmoothStart6(t);
}

float SmoothStep3(float t)
{
	return ComputeCubicBezier1D(0.f, 0.f, 1.f, 1.f, t);
}

float SmoothStep5(float t)
{
	return ComputeQuinticBezier1D(0.f, 0.f, 0.f, 1.f, 1.f, 1.f, t);
}

float Hesitate3(float t)
{
	return ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);
}

float Hesitate5(float t)
{
	return ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);
}

float CustomFunky(float t)
{
	return  Hesitate3(Hesitate5(SmoothStop4(SmoothStart4(t))));
		//ComputeQuinticBezier1D(0.f, 0.f, 1.f, 0.f, 1.f, 1.f, t);
}

Vec3 GetAbsoluteVec3(const Vec3& input)
{
	float x = fabsf(input.x);
	float y = fabsf(input.y);
	float z = fabsf(input.z);
	return Vec3(x, y, z);
}

float MaxNumAmong3(float num1, float num2, float num3)
{
	return fmaxf(fmaxf(num1, num2), num3);
}

float MinNumAmong3(float num1, float num2, float num3)
{
	return fminf(fminf(num1, num2), num3);
}

float Max(float num1, float num2)
{
	return fmaxf(num1, num2);
}

float Min(float num1, float num2)
{
	return fminf(num1, num2);
}

Rgba8 LerpColor(Rgba8 a, Rgba8 b, float alpha)
{
	float colorA[4];
	float colorB[4];
	float colorC[4];
	a.GetAsFloats(colorA);
	b.GetAsFloats(colorB);
	colorC[0] = Interpolate(colorA[0], colorB[0], alpha);
	colorC[1] = Interpolate(colorA[1], colorB[1], alpha);
	colorC[2] = Interpolate(colorA[2], colorB[2], alpha);
	colorC[3] = Interpolate(colorA[3], colorB[3], alpha);
	Rgba8 color;
	color.SetAsFloats(colorC);
	return color;
}

double Pow(double base, int exponent)
{
	double res = 1.0;
	if (exponent <= 0) {
		base = 1.0 / base;
	}
	for (int i = 0; i < exponent; i++) {
		res *= base;
	}
	return res;
}

float Pow(float base, int exponent)
{
	float res = 1.0f;
	if (exponent <= 0) {
		base = 1.0f / base;
	}
	for (int i = 0; i < exponent; i++) {
		res *= base;
	}
	return res;
}

float Log2(float f)
{
	return log2f(f);
}

float NextPowerOfTwo(float number)
{
	float value = Log2(number);
	float floor = (float)RoundDownToInt(value);
	if (value != floor)
		floor += 1.f;
	return Pow(2.f, RoundDownToInt(floor));
}

int Abs(int number)
{
	if (number < 0)
		number = -number;
	return number;
}

float Absf(float number)
{
	return fabsf(number);
}
