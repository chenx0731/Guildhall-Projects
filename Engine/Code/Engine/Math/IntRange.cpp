#include "Engine/Math/IntRange.hpp"

const IntRange IntRange::ZERO		 = IntRange();
const IntRange IntRange::ONE		 = IntRange(1, 1);
const IntRange IntRange::ZERO_TO_ONE = IntRange(0, 1);

IntRange::IntRange(int min, int max)
{
	m_min = min;
	m_max = max;
}

bool IntRange::operator==(const IntRange& compare) const
{
	if (m_min == compare.m_min && m_max == compare.m_max)
		return true;
	return false;
}

bool IntRange::operator!=(const IntRange& compare) const
{
	if (m_min != compare.m_min || m_max != compare.m_max)
		return true;
	return false;
}

void IntRange::operator=(const IntRange& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool IntRange::IsOnRange(int number)
{
	if (number >= m_min && number <= m_max)
		return true;
	return false;
}

bool IntRange::IsOverlappingWith(IntRange range)
{
	if (IsOnRange(range.m_min) || IsOnRange(range.m_max))
		return true;
	return false;
}
