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
// ID�� ��������Ʈ�� �����ϰ� �ĺ� �� �� �ְ� �ϴ� �ĺ�����
// uint64_t �ڷ����� ����Ͽ� ID�� �� 18.4���� ������ ���� ID �浹�� ���
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
// ������ ������ Dynamic Delegate
// unordered_map���� �Լ� ������ �ؽ��Ͽ� ������ ������ �� �ֵ��� �ý��� ����
// --------------------------------------------
template<typename... Args>
class FDynamicDelegate
{
public:
    using HandlerType = function<void(Args...)>;

    //���ܰ� �߻��ϰų� ���������� ����� �����ϰ� ��������Ʈ ����
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
        // Copy-safe strategy: ��� �� ����, ��� ���� �� ȣ��
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
// ��������Ʈ �̺�Ʈ ��ũ��
// DECLARE_DYNAMIC_DELEGATE_OneParam( ��������Ʈ �̸�, �Ķ���� Ÿ�� ) ���� ����
// --------------------------------------------
#define DECLARE_DYNAMIC_DELEGATE(DelegateName) \
    class DelegateName : public FDynamicDelegate<> {}

#define DECLARE_DYNAMIC_DELEGATE_OneParam(DelegateName, Param1Type) \
    class DelegateName : public FDynamicDelegate<Param1Type> {}

#define DECLARE_DYNAMIC_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) \
    class DelegateName : public FDynamicDelegate<Param1Type, Param2Type> {}

#define DECLARE_EVENT(EventName, ...) \
    class EventName : public FDynamicDelegate<__VA_ARGS__> {}