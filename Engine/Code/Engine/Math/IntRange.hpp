#pragma once
struct IntRange
{
	int m_min;
	int m_max;

	IntRange() { m_min = 0; m_max = 0; };
	IntRange(int min, int max);

	bool	operator==(const IntRange& compare) const;		// IntRange == IntRange
	bool	operator!=(const IntRange& compare) const;		// IntRange != IntRange
	void	operator=(const IntRange& copyFrom);				// IntRange = IntRange

	bool	IsOnRange(int number);
	bool	IsOverlappingWith(IntRange range);

	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;

};