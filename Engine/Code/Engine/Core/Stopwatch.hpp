#pragma once

class Clock;

class Stopwatch
{
public:
	Stopwatch();
	explicit Stopwatch(float duration);
	Stopwatch(const Clock* clock, float duration);
	void Start();
	void Restart();
	void Stop();
	float GetElapsedTime() const;
	float GetElapsedFraction() const;
	bool IsStopped() const;
	bool HasDurationElapsed() const;

	bool DecrementDurationIfElapsed();

	const Clock* m_clock = nullptr;
	float m_startTime = 0.f;
	float m_duration = 0.f;
}; 