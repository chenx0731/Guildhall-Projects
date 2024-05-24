#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Clock.hpp"

Stopwatch::Stopwatch()
{
}

Stopwatch::Stopwatch(float duration)
{
	m_duration = duration;
}

Stopwatch::Stopwatch(const Clock* clock, float duration)
{
	m_clock = clock;
	m_duration = duration;
}

void Stopwatch::Start()
{
	m_startTime = m_clock->GetTotalSeconds();
}

void Stopwatch::Restart()
{
	//if (!IsStopped())
	m_startTime = m_clock->GetTotalSeconds();
}

void Stopwatch::Stop()
{
	m_startTime = -99.f;
}

float Stopwatch::GetElapsedTime() const
{
	if (m_startTime == 0.f || m_startTime == -99.f)
		return 0.0f;
	else return m_clock->GetTotalSeconds() - m_startTime;
}

float Stopwatch::GetElapsedFraction() const
{
	return GetElapsedTime() / m_duration;
}

bool Stopwatch::IsStopped() const
{
	if (m_startTime ==-99.f)
		return true;
	return false;
}

bool Stopwatch::HasDurationElapsed() const
{
	if (GetElapsedTime() > m_duration && !IsStopped()) {
		return true;
	}
	return false;
}

bool Stopwatch::DecrementDurationIfElapsed()
{
	if (HasDurationElapsed()) {
		m_startTime += m_duration;
		return true;
	}
	return false;
}
