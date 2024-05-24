#pragma once
struct FloatRange
{
	float m_min;
	float m_max;

	FloatRange() { m_min = 0.f; m_max = 0.f; };
	FloatRange(float min, float max);

	bool	operator==(const FloatRange& compare) const;		// FloatRange == FloatRange
	bool	operator!=(const FloatRange& compare) const;		// FloatRange != FloatRange
	void	operator=(const FloatRange& copyFrom);				// FloatRange = FloatRange

	bool	IsOnRange(float number);
	bool	IsOverlappingWith(FloatRange range);
	void	SetFromText(char const* text, char const* split);

	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;
	
};