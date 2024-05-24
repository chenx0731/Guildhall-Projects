#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>

const IntVec3 IntVec3::Zero = IntVec3(0, 0, 0);

IntVec3::IntVec3(const IntVec3& copyForm)
{
	x = copyForm.x;
	y = copyForm.y;
	z = copyForm.z;
}

IntVec3::IntVec3(int initialX, int initialY, int initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

float IntVec3::GetLength() const
{
	float floatX = static_cast<float> (x);
	float floatY = static_cast<float> (y);
	float floatZ = static_cast<float> (z);
	return sqrtf(floatX * floatX + floatY * floatY + floatZ * floatZ);
}

int IntVec3::GetTaxicabLength() const
{
	return abs(x) + abs(y) + abs(z);
}

int IntVec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

void IntVec3::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ",");
	if (strings.size() < 3)
		ERROR_AND_DIE("Lack of variables when set IntVec3");
	x = atoi(strings[0].c_str());
	y = atoi(strings[1].c_str());
	z = atoi(strings[2].c_str());
}

void IntVec3::operator=(const IntVec3& copyForm)
{
	x = copyForm.x;
	y = copyForm.y;
	z = copyForm.z;
}

bool IntVec3::operator==(const IntVec3& compare) const
{
	if (x == compare.x && y == compare.y && z == compare.z)
		return true;
	return false;
}
std::size_t IntVec3::operator()(const IntVec3& k) const
{
	const unsigned int p1 = 73856093 * k.x;
	const unsigned int p2 = 19349663 * k.y;
	const unsigned int p3 = 83492791 * k.z;
	return p1 + p2 + p3;
}
//bool IntVec3::operator()(const IntVec3& _Left, const IntVec3& _Right) const
//{
//	return _Left == _Right;
//}
bool IntVec3::operator!=(const IntVec3& compare) const
{
	if (x == compare.x && y == compare.y && z == compare.z)
		return false;
	return true;
}

const IntVec3 IntVec3::operator+(const IntVec3& vecToAdd) const
{
	return IntVec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const IntVec3 IntVec3::operator-(const IntVec3& vecToSubtract) const
{
	return IntVec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}
