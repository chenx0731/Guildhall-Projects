#include "Engine/Math/IntVec2.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const IntVec2 IntVec2::Zero = IntVec2(0, 0);

IntVec2::IntVec2(const IntVec2& copyForm)
{
	x = copyForm.x;
	y = copyForm.y;
}

IntVec2::IntVec2(int initialX, int initialY)
{
	x = initialX;
	y = initialY;
}

float IntVec2::GetLength() const
{
	float floatX = static_cast<float> (x);
	float floatY = static_cast<float> (y);
	return sqrtf(floatX * floatX + floatY * floatY);
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

int IntVec2::GetLengthSquared() const
{
	return x * x + y * y;
}

float IntVec2::GetOrientationRadians() const
{
	float floatX = static_cast<float> (x);
	float floatY = static_cast<float> (y);
	return atan2f(floatY, floatX);
}

float IntVec2::GetOrientationDegrees() const
{
	return GetOrientationRadians() * 180.f / 3.141592653589793f;
}

IntVec2 const IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y, x);
}

IntVec2 const IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y, -x);
}

void IntVec2::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ",");
	if (strings.size() < 2)
		ERROR_AND_DIE("Lack of variables when set IntVec2");
	x = atoi(strings[0].c_str());
	y = atoi(strings[1].c_str());
}

void IntVec2::Rotate90Degrees()
{
	int temp = x;
	x = -y;
	y = temp;
}

void IntVec2::RotateMinus90Degrees()
{
	int temp = x;
	x = y;
	y = -temp;
}

void IntVec2::operator=(const IntVec2& copyForm)
{
	x = copyForm.x;
	y = copyForm.y;
}

bool IntVec2::operator==(const IntVec2& compare) const
{
	if (x == compare.x && y == compare.y)
		return true;
	return false;
}

bool IntVec2::operator!=(const IntVec2& compare) const
{
	if (x == compare.x && y == compare.y)
		return false;
	return true;
}

const IntVec2 IntVec2::operator+(const IntVec2& vecToAdd) const
{
	return IntVec2(x + vecToAdd.x, y + vecToAdd.y);
}

const IntVec2 IntVec2::operator-(const IntVec2& vecToSubtract) const
{
	return IntVec2(x - vecToSubtract.x, y - vecToSubtract.y);
}

bool IntVec2::operator<(const IntVec2& compare) const
{
	if (x < compare.x)
		return true;
	else if (x == compare.x)
	{
		if (y < compare.y)
			return true;
	}
	return false;
}

bool IntVec2::operator>(const IntVec2& compare) const
{
	if (x > compare.x)
		return true;
	else if (x == compare.x)
	{
		if (y > compare.y)
			return true;
	}
	return false;
}
