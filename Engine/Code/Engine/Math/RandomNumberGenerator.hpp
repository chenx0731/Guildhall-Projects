#pragma once
class RandomNumberGenerator
{
public:
	int RollRandomIntLessThan(int maxNotInclusive);
	int RollRandomIntInRange(int minInclusive, int maxInclusive);
	float RollRandomFloatZeroToOne();
	float RollRandomFloatInRange(float minInclusive, float maxInclusive);
	float RollRandomFloatInAbsRange(float minInclusive, float maxInclusive);
public:
	int				m_position = 0;
	unsigned int	m_seed = 0;
};