#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const FloatRange FloatRange::ZERO		 = FloatRange();
const FloatRange FloatRange::ONE		 = FloatRange(1.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ONE = FloatRange(0.f, 1.f);

FloatRange::FloatRange(float min, float max)
{
	m_min = min;
	m_max = max;
}

bool FloatRange::operator==(const FloatRange& compare) const
{
	if (m_min == compare.m_min && m_max == compare.m_max)
		return true;
	return false;
}

bool FloatRange::operator!=(const FloatRange& compare) const
{
	if (m_min != compare.m_min || m_max != compare.m_max)
		return true;
	return false;
}

void FloatRange::operator=(const FloatRange& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool FloatRange::IsOnRange(float number)
{
	if (number > m_min && number < m_max)
		return true;
	return false;
}

bool FloatRange::IsOverlappingWith(FloatRange range)
{
	if (IsOnRange(range.m_min + FLT_EPSILON) || IsOnRange(range.m_max - FLT_EPSILON) || range.IsOnRange(m_min + FLT_EPSILON) || range.IsOnRange(m_max - FLT_EPSILON))
		return true;
	return false;
}

void FloatRange::SetFromText(char const* text, char const* split)
{
	Strings strings = SplitStringOnDelimiter(text, split);
	if (strings.size() < 2)
		ERROR_AND_DIE("Lack of variables when set FloatRange");
	m_min = static_cast<float>(atof(strings[0].c_str()));
	m_max = static_cast<float>(atof(strings[1].c_str()));
}


