#pragma once

#include "Engine/Math/Vec2.hpp"

struct Vec3;
struct Vec4;
struct IntVec2;
struct AABB2;
struct AABB3;
struct Capsule2;
struct OBB2;
struct LineSegment2;
struct RaycastResult2D;
struct RaycastResult3D;
struct Mat44;
struct Rgba8;

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_CAMERA_FACING,
	WORLD_UP_CAMERA_OPPOSING,
	FULL_CAMERA_FACING,
	FULL_CAMERA_OPPOSING,

	COUNT
};

// Angle utilities
float		ConvertDegreesToRadians(float degrees);
float		ConvertRadiansToDegrees(float radians);
float		CosDegrees(float degrees);
float		SinDegrees(float degrees);
float		Atan2Degrees(float y, float x);
float		TanDegrees(float degrees);
float		GetShortestAngularDispDegrees(float startDegrees, float endDegrees);
float		GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float		GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);
float		GetAngleDegreesWithin360(float degree);

float		GetDistance2D(Vec2 const& positionA, Vec2 const& positionB);
float		GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB);
float		GetDistance3D(Vec3 const& positionA, Vec3 const& positionB);
float		GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB);
float		GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float		GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);
int			GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB);
float		GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
Vec2 const	GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
float		GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto);
Vec3 const	GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto);

// Geometric queries
bool		IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);
bool		IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box);
bool		IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule);
bool		IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool		IsPointInsideOBB2D(Vec2 const& point, OBB2 const& orientedBox);
bool		IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius);
bool		IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius);

bool		DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool		DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);

Vec2 const	GetNearestPointOnDisc2D(Vec2 const& referencePos, Vec2 const& disCenter, float discRadius);
Vec2 const	GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& box);
Vec2 const	GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, LineSegment2 const& infiniteLine);
Vec2 const	GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine);
Vec2 const	GetNearestPointOnLineSegment2D(Vec2 const& referencePos, LineSegment2 const& lineSegment);
Vec2 const	GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 const& lineSegStart, Vec2 const& lineSegEnd);
Vec3 const	GetNearestPointOnLineSegment3D(Vec3 const& referencePos, Vec3 const& lineSegStart, Vec3 const& lineSegEnd);
Vec2 const	GetNearestPointOnCapsule2D(Vec2 const& referencePos, Capsule2 const& capsule);
Vec2 const	GetNearestPointOnCapsule2D(Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
//Vec3 const	GetNearestPointOnCapsule3D(Vec3 const& referencePos, Vec3 const& boneStart, Vec3 const& boneEnd, float radius);
Vec2 const	GetNearestPointOnOBB2D(Vec2 const& referencePos, OBB2 const& orientedBox);

bool		PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint);
bool		PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool		PushDiscOutOfFixedDisc3D(Vec3& mobileDiscCenter, float mobileDiscRadius, Vec3 const& fixedDiscCenter, float fixedDiscRadius);
bool		PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius);
bool		PushDiscsOutOfEachOther3D(Vec3& aCenter, float aRadius, Vec3& bCenter, float bRadius);
bool		PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);
bool		PushAABB2DOutOfFixedPoint2D(AABB2& mobileAABB, Vec2 const& fixedPoint);
bool		PushAABB2DOutOfFixedAABB2D(AABB2& mobileAABB, AABB2 const& fixedAABB);
bool		PushCylinder3DOutOfAABB3D(Vec3& cylinderBottomCenter, float cylinderRadius, float cylinderHeight, AABB3& aabb3, bool isCldMovable = false, bool isAABBMovable = false);
bool		PushAABB3DOutOfFixedPoint3D(AABB3& mobileAABB, Vec3 const& fixedPoint);
bool		PushAABB3DOutOfFixedAABB3D(AABB3& mobileAABB, AABB3 const& fixedAABB);

bool		BounceDiscOffFixedPoint2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const Vec2& point, float mobileElasticity, float fixedElasticity);
bool		BounceDiscOffEachOther2D(Vec2& aCenter, float aRadius, Vec2& aVel, Vec2& bCenter, float bRadius, Vec2& bVel, float aElasticity = 1.f, float bElasticity = 1.f);
bool		BounceDiscOffEachOther3D(Vec3& aCenter, float aRadius, Vec3& aVel, Vec3& bCenter, float bRadius, Vec3& bVel, float aElasticity = 1.f, float bElasticity = 1.f);
bool		BounceDiscOffFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, Vec2& fixedDiscCenter, float fixedDiscRadius, float aElasticity = 1.f, float bElasticity = 1.f);
bool		BounceDiscOffFixedDisc3D(Vec3& mobileDiscCenter, float mobileDiscRadius, Vec3& mobileDiscVel, Vec3& fixedDiscCenter, float fixedDiscRadius, float aElasticity = 1.f, float bElasticity = 1.f);
bool		BounceDiscOffFixedOBB2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const OBB2& fixedOBB, float mobileElasticity, float fixedElasticity);
bool		BounceDiscOffFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel, const Capsule2& fixedCapsule, float mobileElasticity, float fixedElasticity);

void		TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation);
void		TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void		TransformPositionXY3D(Vec3& posToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY);
void		TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void		TransformPositionXYZ3D(Vec3& posToTransform, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation);

float		GetClamped(float value, float minValue, float maxValue);
int			GetClamped(int value, int minValue, int maxValue);
float		GetClampedZeroToOne(float value);
float		Interpolate(float start, float end, float fractionTowardEnd);
float		GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float		RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float		RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int			RoundDownToInt(float value);

float		DotProduct2D(Vec2 const& a, Vec2 const& b);
float		DotProduct3D(Vec3 const& a, Vec3 const& b);
float		DotProduct4D(Vec4 const& a, Vec4 const& b);
float		CrossProduct2D(Vec2 const& a, Vec2 const& b);
Vec3		CrossProduct3D(Vec3 const& a, Vec3 const& b);

float			NormalizeByte(unsigned char byteValue);
unsigned char	DenormalizeByte(float zeroToOne);

RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius);
RaycastResult2D RaycastVsLine2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 line);
RaycastResult2D RaycastVsInfiniteLine2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 line);
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 aabb);
RaycastResult3D RaycastVsCylinderZ3D(const Vec3& start, const Vec3& direction, float distance,
	const Vec2& center, float minZ, float maxZ, float radius);
RaycastResult3D RaycastVsAABB3D(const Vec3& start, const Vec3& direction, float distance, AABB3 aabb);

void		SwapFloatValue(float& a, float& b);

Mat44 GetBillboardMatrix(BillboardType billboardType,
	Mat44 const& cameraMatrix,
	const Vec3& billboardPosition,
	const Vec2& billboardScale = Vec2(1.f, 1.f));

float ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);

float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStart6(float t);

float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);
float SmoothStop6(float t);

float SmoothStep3(float t);
float SmoothStep5(float t);

float Hesitate3(float t);
float Hesitate5(float t);
float CustomFunky(float t);

Vec3 GetAbsoluteVec3(const Vec3& input);

float MaxNumAmong3(float num1, float num2, float num3);
float MinNumAmong3(float num1, float num2, float num3);

float Max(float num1, float num2);
float Min(float num1, float num2);

Rgba8 LerpColor(Rgba8 a, Rgba8 b, float alpha);

double Pow(double base, int exponent);
float Pow(float base, int exponent);

float Log2(float f);

float NextPowerOfTwo(float number);

int Abs(int number);

float Absf(float number);