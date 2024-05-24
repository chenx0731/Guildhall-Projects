#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include <stdlib.h>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	unsigned int randomUInt = Get1dNoiseUint(m_position++, m_seed);
	return randomUInt % maxNotInclusive;
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	unsigned int randomUInt = Get1dNoiseUint(m_position++, m_seed);
	return minInclusive + randomUInt % (maxInclusive - minInclusive + 1);
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	unsigned int randomUInt = Get1dNoiseUint(m_position++, m_seed);
	constexpr unsigned int MAX_RANDOM_UINT = 0xffffffff;
	constexpr double ONE_OVER_MAX_RANDOM_UINT = 1.0 / (double)MAX_RANDOM_UINT;
	return float((double)(randomUInt) * ONE_OVER_MAX_RANDOM_UINT);
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	return minInclusive + RollRandomFloatZeroToOne() * (maxInclusive - minInclusive);
		//(float)(rand()) / (float)(RAND_MAX / (maxInclusive - minInclusive));
}

float RandomNumberGenerator::RollRandomFloatInAbsRange(float minInclusive, float maxInclusive)
{
	float num = RollRandomFloatInRange(minInclusive, maxInclusive);
	float sign = RollRandomFloatInRange(-1.f, 1.f);
	if (sign < 0.f) return -num;
	return num;
}
