#pragma once
#define UNUSED(x) (void)(x);
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//#include "Engine/Core/EngineCommon.hpp"
const float PI = 3.141592653589793f;

//-----------------------------------------------------------------------------------------------
Vec4::Vec4( const Vec4& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
	, w( copy.w )
{
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}

Vec4::Vec4(const Vec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = 0.f;
}

Vec4::Vec4(const Vec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = 0.f;
	w = 0.f;
}

void Vec4::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ",");
	if (strings.size() < 4)
		ERROR_AND_DIE("Lack of variables when set Vec4");
	x = static_cast<float>(atof(strings[0].c_str()));
	y = static_cast<float>(atof(strings[1].c_str()));
	z = static_cast<float>(atof(strings[2].c_str()));
	w = static_cast<float>(atof(strings[3].c_str()));
}



//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator + ( const Vec4& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-() const
{
	return Vec4( -x, -y, -z, -w );
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( float uniformScale ) const
{
	return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( const Vec4& vecToMultiply ) const
{
	return Vec4( x * vecToMultiply.x, y * vecToMultiply.y , z * vecToMultiply.z, w * vecToMultiply.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/( float inverseScale ) const
{
	return Vec4( x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=( const Vec4& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=( const Vec4& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*( float uniformScale, const Vec4& vecToScale )
{
	return Vec4( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z, uniformScale * vecToScale.w );
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==( const Vec4& compare ) const
{
	if (x == compare.x && y == compare.y && z == compare.z && w == compare.w)
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=( const Vec4& compare ) const
{
	if (x == compare.x && y == compare.y && z == compare.z && w == compare.w)
	{
		return false;
	}
	return true;
}


