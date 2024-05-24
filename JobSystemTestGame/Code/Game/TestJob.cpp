#include "Game/TestJob.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <thread>


TestJob::TestJob(IntVec2 coords) : Job()
{
	m_coords = coords;
	m_sleepTime = g_rng->RollRandomIntInRange(50, 3000);
}

void TestJob::Execute()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
}
