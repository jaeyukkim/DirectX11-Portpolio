#include "HeaderCollection.h"
#include "Timer.h"

Timer*  Timer::Instance = nullptr;

void Timer::Create()
{
	if (Instance == nullptr)
		Instance = new Timer();

}

void Timer::Destroy()
{
	Delete(Instance);
}

Timer* Timer::Get()
{
	return Instance;
	
}

Timer::Timer()
{
	StartTime = steady_clock::now();
	PrevFrameTime = StartTime;
}

Timer::~Timer()
{
}

void Timer::Tick()
{
	if (TimeData.bPaused)
	{
		TimeData.DeltaTime = 0.0f;
		return;
	}

	steady_clock::time_point current = steady_clock::now();
	duration<double> elapsed = current - PrevFrameTime;

	TimeData.DeltaTime = bUseFixedDeltaTime ? FixedDelta : static_cast<float>(elapsed.count());

	PrevFrameTime = current;
	TimeData.RunningTime += TimeData.DeltaTime;
	FrameCount++;
}

void Timer::Reset()
{
	StartTime = steady_clock::now();
	PrevFrameTime = StartTime;
	TimeData.RunningTime = 0.0f;
	FrameCount = 0;
}

void Timer::Pause()
{
	TimeData.bPaused = true;
}

void Timer::Resume()
{
	TimeData.bPaused = false;
}

bool Timer::IsPaused() const
{
	return TimeData.bPaused;
}

void Timer::SetFixedDeltaTime(float value)
{
	bUseFixedDeltaTime = true;
	FixedDelta = value;
}

void Timer::UseRealDeltaTime()
{
	bUseFixedDeltaTime = false;
}

float Timer::GetFixedDeltaTime() const
{
	return FixedDelta;
}