#pragma once

class Timer
{



public:
	static void Create();
	static void Destroy();
	static Timer* Get();

public:
	void Tick();
	void Reset();

	float GetDeltaTime() const { return DeltaTime; }
	float GetRunningTime() const { return RunningTime; }
	uint64_t GetFrameCount() const { return FrameCount; }

	void Pause();
	void Resume();
	bool IsPaused() const;

	void SetFixedDeltaTime(float value);
	void UseRealDeltaTime();
	float GetFixedDeltaTime() const;

private:
	Timer();
	~Timer();


private:
	static Timer* Instance;

private:
	steady_clock::time_point StartTime;
	steady_clock::time_point PrevFrameTime;

	float DeltaTime = 0.0f;
	float RunningTime = 0.0f;
	uint64_t FrameCount = 0;

	bool bPaused = false;

	bool bUseFixedDeltaTime = false;
	float FixedDelta = 0.016f; // 60fps
};