#pragma once
#define UNUSED(x) (void)(x);
#include "Engine/Math/Vec2.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//#include "Engine/Core/EngineCommon.hpp"
const float PI = 3.141592653589793f;

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

void Vec2::SetFromText(char const* text, char const* split)
{
	Strings strings = SplitStringOnDelimiter(text, split);
	if (strings.size() < 2)
		ERROR_AND_DIE("Lack of variables when set Vec2");
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
}


// Static methods (e.g. creation functions)
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	return Vec2(length * cosf(orientationRadians), length * sinf(orientationRadians));
}

Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{
	return Vec2(length * cosf(orientationDegrees * PI / 180.f), length * sinf(orientationDegrees * PI / 180.f));
}

// Accessors (const methods)
float Vec2::GetLength() const
{
	return sqrtf(x * x + y * y);
}

float Vec2::GetLengthSquared() const
{
	return x * x + y * y;
}

float Vec2::GetOrientationRadians() const
{
	return atan2f(y, x);
}

float Vec2::GetOrientationDegrees() const
{
	return atan2f(y, x) * 180.f / PI;
}
Vec2 const Vec2::GetRotated90Degrees() const
{
	return Vec2(-y, x);
}
Vec2 const Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2(y, -x);
}
Vec2 const Vec2::GetRotatedRadians(float deltaRadians) const
{
	return Vec2(GetLength() * cosf(GetOrientationRadians() + deltaRadians), GetLength() * sinf(GetOrientationRadians() + deltaRadians));
}
Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees) const
{
	return Vec2(GetLength() * cosf((GetOrientationDegrees() + deltaDegrees) * PI / 180.f), GetLength() * sinf((GetOrientationDegrees() + deltaDegrees) * PI / 180.f));
}
Vec2 const Vec2::GetClamped(float maxLength) const
{
	float length = GetLength();
	if (length > maxLength)
	{
		float scale = maxLength / length;
		return Vec2(x * scale, y * scale);
	}
	else return Vec2(x, y);
}
Vec2 const Vec2::GetNormalized() const
{
	Vec2 result;
	float length = GetLength();
	if (length != 0.f) {
		result.x = x / length;
		result.y = y / length;
	}
	return result;
}

Vec2 const Vec2::GetReflected(const Vec2& platformVec) const
{
	Vec2 projectionOnPlatform = DotProduct2D(Vec2(x, y), platformVec) * platformVec;
	Vec2 verticalToPlatform = Vec2(x, y) - projectionOnPlatform;
	return verticalToPlatform - projectionOnPlatform;
}

// Mutators (non-const methods)
void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	float length = GetLength();
	x = length * cosf(newOrientationRadians);
	y = length * sinf(newOrientationRadians);
}

void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	float length = GetLength();
	x = length * cosf(newOrientationDegrees * PI / 180.f);
	y = length * sinf(newOrientationDegrees * PI / 180.f);
}

void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	x = newLength * cosf(newOrientationRadians);
	y = newLength * sinf(newOrientationRadians);
}

void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	x = newLength * cosf(newOrientationDegrees * PI / 180.f);
	y = newLength * sinf(newOrientationDegrees * PI / 180.f);
}

void Vec2::Rotate90Degrees()
{
	float temp = x;
	x = -y;
	y = temp;
}

void Vec2::RotateMinus90Degrees()
{
	float temp = x;
	x = y;
	y = -temp;
}

void Vec2::RotateRadians(float deltaRadians)
{
	SetOrientationRadians(GetOrientationRadians() + deltaRadians);
}

void Vec2::RotateDegrees(float deltaDegrees)
{
	SetOrientationDegrees(GetOrientationDegrees() + deltaDegrees);
}

void Vec2::SetLength(float newLength)
{
	float radians = GetOrientationRadians();
	x = newLength * cosf(radians);
	y = newLength * sinf(radians);
}

void Vec2::ClampLength(float maxLength)
{
	float length = GetLength();
	if (length > maxLength)
	{
		float scale = maxLength / length;
		x *= scale;
		y *= scale;
	}
}

void Vec2::Normalize()
{
	float length = GetLength();
	if (length == 0.f)
		return;
	x /= length;
	y /= length;
}

void Vec2::Reflect(const Vec2& platformVec)
{
	Vec2 temp = GetReflected(platformVec);
	x = temp.x;
	y = temp.y;
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float prevLength = GetLength();
	x /= prevLength;
	y /= prevLength;
	return prevLength;
}


void Vec2::RotateAboutPoint(Vec2 point, float degree)
{
	Vec2 pointToVert = *this - point;
	pointToVert.RotateDegrees(degree);
	*this = point + pointToVert;
}

void Vec2::ScaleAboutPoint(Vec2 point, float scale)
{
	Vec2 pointToVert = *this - point;
	pointToVert *= scale;
	*this = point + pointToVert;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if (x == compare.x && y == compare.y)
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if (x == compare.x && y == compare.y)
	{
		return false;
	}
	return true;
}


