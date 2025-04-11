#include "HeaderCollection.h"


TimerManager& TimerManager::Get()
{
    static TimerManager instance;
    return instance;
}

TimerHandle TimerManager::SetTimer(float delaySeconds, CallbackFunc callback, bool looping)
{
    TimerHandle handle = ++IDCounter;
    Timers[handle] = TimerData{ delaySeconds, delaySeconds, callback, looping, true };
    return handle;
}

void TimerManager::ClearTimer(TimerHandle handle)
{
    Timers.erase(handle);
}

void TimerManager::ClearAllTimers()
{
    Timers.clear();
}

bool TimerManager::IsTimerActive(TimerHandle handle) const
{
    auto it = Timers.find(handle);
    return it != Timers.end() && it->second.bActive;
}

void TimerManager::Tick()
{
    float deltaTime = Timer::Get()->GetDeltaTime();

    std::vector<TimerHandle> expiredTimers;

    for (auto& [handle, timer] : Timers)
    {
        if (!timer.bActive)
            continue;

        timer.RemainingTime -= deltaTime;
        if (timer.RemainingTime <= 0.0f)
        {
            if (timer.Callback)
                timer.Callback();

            if (timer.Looping)
            {
                timer.RemainingTime = timer.OriginalDelay;
            }
            else
            {
                expiredTimers.push_back(handle);
            }
        }
    }

    for (TimerHandle handle : expiredTimers)
    {
        Timers.erase(handle);
    }
}