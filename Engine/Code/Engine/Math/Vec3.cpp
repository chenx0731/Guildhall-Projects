#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
const float PI = 3.141592653589793f;

Vec3::Vec3(const Vec3& copyFrom) // copy constructor (from another vec3)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

Vec3::Vec3(const Vec2& copyFrom, float Z)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = Z;
}

Vec3::Vec3(float initialX, float initialY, float initialZ)		// explicit constructor (from x, y, z)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

//Vec3::Vec3(int X, int Y, int Z)
//{
//	x = float(X);
//	y = float(Y);
//	z = float(Z);
//}

void Vec3::SetFromText(char const* text, char const* split)
{
	Strings strings = SplitStringOnDelimiter(text, split);
	if (strings.size() < 3)
		ERROR_AND_DIE("Lack of variables when set Vec3");
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
	z = static_cast<float>(atof(strings[2].c_str()));
}

const Vec3 Vec3::MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length)
{
	float latitudeDegrees = ConvertRadiansToDegrees(latitudeRadians);
	float longtitudeDegrees = ConvertRadiansToDegrees(longitudeRadians);
	return MakeFromPolarDegrees(latitudeDegrees, longtitudeDegrees, length);
}

const Vec3 Vec3::MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length)
{
	float cosLatitude = CosDegrees(latitudeDegrees);
	float sinLatitude = SinDegrees(latitudeDegrees);
	float cosLongitude = CosDegrees(longitudeDegrees);
	float sinLongitude = SinDegrees(longitudeDegrees);
	float z = length * sinLatitude;
	float xy = length * cosLatitude;
	float x = xy * cosLongitude;
	float y = xy * sinLongitude;

	return Vec3(x, y, z);
}

// Accessors (const methods)
float Vec3::GetLength() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vec3::GetLengthXY() const
{
	return sqrtf(x * x + y * y);
}

float Vec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

float Vec3::GetLengthXYSquared() const
{
	return x * x + y * y;
}

float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}

float Vec3::GetAngleAboutZDegrees() const
{
	return atan2f(y, x) * 180.f / PI;
}

Vec3 const Vec3::GetRotatedAboutZRadians(float deltaRadians) const
{
	float angleAboutZRadians = GetAngleAboutZRadians();
	float lengthXY = GetLengthXY();
	return Vec3(lengthXY * cosf(angleAboutZRadians + deltaRadians), lengthXY * sinf(angleAboutZRadians + deltaRadians) , z);
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float deltaDegrees) const
{
	float angleAboutZDegrees = GetAngleAboutZDegrees();
	float lengthXY = GetLengthXY();
	return Vec3(lengthXY * cosf((angleAboutZDegrees + deltaDegrees) * PI / 180.f), lengthXY * sinf((angleAboutZDegrees + deltaDegrees) * PI / 180.f), z);
}

Vec3 const Vec3::GetClamped(float maxLength) const
{
	float length = GetLength();
	if (length > maxLength)
	{
		float scale = maxLength / length;
		return Vec3(x * scale, y * scale, z * scale);
	}
	else return Vec3(x, y, z);
}

Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	if (length == 0.f)
		return Vec3(0.f, 0.f, 0.f);
	return Vec3(x / length, y / length, z / length);
}
Vec2 Vec3::GetVec2XY() const
{
	return Vec2(x, y);
}

void Vec3::GetFloat4(float* outResult) const
{
	outResult[0] = x;
	outResult[1] = y;
	outResult[2] = z;
	outResult[3] = 0.f;
}

// Operators (const)
bool Vec3::operator==(Vec3 const& compare) const
{
	if (x == compare.x && y == compare.y && z == compare.z)
	{
		return true;
	}
	else return false;
}

bool Vec3::operator!=(Vec3 const& compare) const
{
	if (x == compare.x && y == compare.y && z == compare.z)
	{
		return false;
	}
	else return true;
}

Vec3 const Vec3::operator+(Vec3 const& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

Vec3 const Vec3::operator-(Vec3 const& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

Vec3 const Vec3::operator*(float uniformScale) const
{
	return Vec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

Vec3 const Vec3::operator/(float inverseScale) const
{
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vec3::Normalize()
{
	float length = GetLength();
	if (length == 0.f)
		return;
	x /= length;
	y /= length;
	z /= length;
}


// Operator (self-mutating / non-const)
void Vec3::operator+=(Vec3 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void Vec3::operator-=(Vec3 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void Vec3::operator*=(float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void Vec3::operator/=(float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

void Vec3::operator=(Vec3 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool Vec3::operator<(const Vec3& compare) const
{
	if (x < compare.x)
		return true;
	else if (x == compare.x)
	{
		if (y < compare.y)
			return true;
		else if (y == compare.y) {
			if (z < compare.z)
				return true;
		}
	}
	return false;
}



// Standalone "friend" functions that are conceptually, but not actually, part of Vec3::
Vec3 const operator*(float uniformScale, Vec3 const& vecToScale)
{
	return Vec3(uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z);
}
