#pragma once

struct TimeDesc
{
	float DeltaTime = 0.0f;
	float RunningTime = 0.0f;
	bool bPaused = false;
	float padding = 0.0f;
};

class Timer
{


public:
	static void Create();
	static void Destroy();
	static Timer* Get();

public:
	void Tick();
	void Reset();

	float GetDeltaTime() const { return TimeData.DeltaTime; }
	float GetRunningTime() const { return TimeData.RunningTime; }
	uint64_t GetFrameCount() const { return FrameCount; }

	void Pause();
	void Resume();
	bool IsPaused() const;

	void SetFixedDeltaTime(float value);
	void UseRealDeltaTime();
	float GetFixedDeltaTime() const;
	TimeDesc GetTimeData() {return TimeData;}
private:
	Timer();
	~Timer();


private:
	static Timer* Instance;

private:
	steady_clock::time_point StartTime;
	steady_clock::time_point PrevFrameTime;

	TimeDesc TimeData;


	
	uint64_t FrameCount = 0;


	bool bUseFixedDeltaTime = false;
	float FixedDelta = 0.016f; // 60fps
};