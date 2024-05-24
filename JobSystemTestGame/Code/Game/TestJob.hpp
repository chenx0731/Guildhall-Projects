#pragma once
#include "Engine/Core/Job.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


class TestJob : public Job
{
public:
	TestJob(IntVec2 coords);
	virtual void Execute() override;

public:
	int m_sleepTime = 0;
	IntVec2 m_coords;
};


	