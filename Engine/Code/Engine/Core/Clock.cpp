#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include <algorithm>

static Clock g_systemClock;

Clock::Clock()
{
	if (this != &g_systemClock)
		m_parent = &g_systemClock;
}

Clock::~Clock()
{
	if (m_parent != nullptr)
		m_parent->RemoveChild(this);
	for (int childIndex = 0; childIndex < m_children.size(); childIndex++) {
		m_children[childIndex]->m_parent = nullptr;
	}
}

Clock::Clock(Clock& parent)
{
	m_parent = &parent;
	parent.AddChild(this);
}

void Clock::Reset()
{
	m_lastUpdateTimeInSeconds = 0.f;
	m_totalSeconds = 0.f;
	m_deltaSeconds = 0.f;
	m_frameCount = 0;
}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Unpause()
{
	m_isPaused = false;
}

void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
	m_isPaused = false;
}

void Clock::SetTimeScale(float timeScale)
{
	m_timeScale = timeScale;
}

float Clock::GetTimeScale() const
{
	return m_timeScale;
}

float Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

float Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

size_t Clock::GetFrameCount() const
{
	return m_frameCount;
}

Clock& Clock::GetSystemClock()
{
	return g_systemClock;
}

void Clock::TickSystemClock()
{
	g_systemClock.Tick();
}

void Clock::Tick()
{
	static float lastTime = static_cast<float> (GetCurrentTimeSeconds());
	float currentTime = static_cast<float> (GetCurrentTimeSeconds());
	float deltaSeconds = currentTime - lastTime;
	if (deltaSeconds > m_maxDeltaSeconds)
	{
		deltaSeconds = m_maxDeltaSeconds;
	}
	lastTime = currentTime;
	Advance(deltaSeconds);
}

void Clock::Advance(float deltaTimeSeconds)
{
	if (m_isPaused)
		deltaTimeSeconds = 0.f;
	m_lastUpdateTimeInSeconds = m_totalSeconds;
	m_deltaSeconds = deltaTimeSeconds * m_timeScale;
	m_totalSeconds += deltaTimeSeconds * m_timeScale;
	m_frameCount++;
	for (int childIndex = 0; childIndex < m_children.size(); childIndex++) {
		Clock*& tempClock = m_children[childIndex];
		if (tempClock)
		tempClock->Advance(m_deltaSeconds);
	}
	if (m_stepSingleFrame) {
		m_stepSingleFrame = false;
		m_isPaused = true;
	}
}

void Clock::AddChild(Clock* childClock)
{
	for (int childIndex = 0; childIndex < m_children.size(); childIndex++) {
		if (m_children[childIndex] == nullptr) {
			m_children[childIndex] = childClock;
			return;
		}
	}
	m_children.push_back(childClock);
}

void Clock::RemoveChild(Clock* childClock)
{
	/*for (int childIndex = 0; childIndex < m_children.size(); childIndex++) {
		if (m_children[childIndex] == childClock) {
			m_children[childIndex]->m_parent = nullptr;
			m_children[childIndex] = nullptr;
			return;
		}
	}*/
	m_children.erase(std::remove(m_children.begin(), m_children.end(), childClock), m_children.end());
	/*m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [=](auto const& element) {
		return element == childClock;
		}), m_children.end());*/
	
}
