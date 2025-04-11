#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

using CallbackFunc = std::function<void()>;
using TimerHandle = uint64_t;

class TimerManager
{
public:
    static TimerManager& Get();

    TimerHandle SetTimer(float delaySeconds, CallbackFunc callback, bool looping = false);
    void ClearTimer(TimerHandle handle);
    void ClearAllTimers();
    bool IsTimerActive(TimerHandle handle) const;

    void Tick(); // ¡ç Timer::Get()->GetDeltaTime() »ç¿ë

private:
    TimerManager() = default;
    ~TimerManager() = default;

    struct TimerData
    {
        float RemainingTime;
        float OriginalDelay;
        CallbackFunc Callback;
        bool Looping;
        bool bActive = true;
    };

    std::unordered_map<TimerHandle, TimerData> Timers;
    TimerHandle IDCounter = 0;
};