#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>

using namespace std;


// --------------------------------------------
// ID는 델리게이트를 유일하게 식별 할 수 있게 하는 식별자임
// uint64_t 자료형을 사용하여 ID는 약 18.4경의 범위를 가져 ID 충돌을 방어
// --------------------------------------------
class FDelegateHandle
{
public:
    FDelegateHandle() : ID(++Counter) {}
    uint64_t GetID() const { return ID; }
    bool operator==(const FDelegateHandle& Other) const { return ID == Other.ID; }
    
private:
    uint64_t ID;
    static atomic<uint64_t> Counter;
};




// --------------------------------------------
// 스레드 안전한 Dynamic Delegate
// unordered_map으로 함수 포인터 해싱하여 빠르게 접근할 수 있도록 시스템 구축
// --------------------------------------------
template<typename... Args>
class FDynamicDelegate
{
public:
    using HandlerType = function<void(Args...)>;

    //예외가 발생하거나 스코프에서 벗어나도 안전하게 델리게이트 해제
    FDelegateHandle Add(const HandlerType& handler, const string& debugName = "")
    {
        lock_guard<mutex> lock(Mutex);
        FDelegateHandle handle;
        Bindings[handle.GetID()] = { handler, debugName };
        return handle;
    }

    void Remove(const FDelegateHandle& handle)
    {
        lock_guard<mutex> lock(Mutex);
        Bindings.erase(handle.GetID());
    }

    void Clear()
    {
        lock_guard<mutex> lock(Mutex);
        Bindings.clear();
    }

    void Broadcast(Args... args)
    {
        // Copy-safe strategy: 잠금 후 복사, 잠금 해제 후 호출
        unordered_map<uint64_t, FBinding> snapshot;
        {
            lock_guard<mutex> lock(Mutex);
            snapshot = Bindings;
        }

        for (auto& [id, binding] : snapshot)
        {
            if (binding.Handler)
            {
                binding.Handler(args...);
            }
        }
    }

private:
    struct FBinding
    {
        HandlerType Handler;
        string DebugName;
    };

    unordered_map<uint64_t, FBinding> Bindings;
    mutable mutex Mutex;
};

// --------------------------------------------
// Weak Lambda Binder
// --------------------------------------------
template<typename T, typename... Args>
function<void(Args...)> BindWeakLambda(weak_ptr<T> weakObj, void (T::* func)(Args...))
{
    return [weakObj, func](Args... args)
    {
        if (auto shared = weakObj.lock())
        {
            (shared.get()->*func)(args...);
        }
    };
}

// --------------------------------------------
// 델리게이트 이벤트 매크로
// DECLARE_DYNAMIC_DELEGATE_OneParam( 델리게이트 이름, 파라미터 타입 ) 으로 선언
// --------------------------------------------
#define DECLARE_DYNAMIC_DELEGATE(DelegateName) \
    class DelegateName : public FDynamicDelegate<> {}

#define DECLARE_DYNAMIC_DELEGATE_OneParam(DelegateName, Param1Type) \
    class DelegateName : public FDynamicDelegate<Param1Type> {}

#define DECLARE_DYNAMIC_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) \
    class DelegateName : public FDynamicDelegate<Param1Type, Param2Type> {}

#define DECLARE_EVENT(EventName, ...) \
    class EventName : public FDynamicDelegate<__VA_ARGS__> {}